#include "DetectPlate.h"



bool DetectPlate::sortContours(vector<Point> contours1, vector<Point> contours2)
{
	Rect r1 = boundingRect(contours1), r2 = boundingRect(contours2);
	return (r1.x < r2.x);
}

int DetectPlate::centerX(Rect r)
{
	return (r.x + (int)r.width / 2);
}

int DetectPlate::centerY(Rect r)
{
	return (int) (r.y + (int)r.height / 2);
}



double DetectPlate::distanceBetweenRect(Rect r1, Rect r2) {
	return sqrt((centerX(r2) - centerX(r1)) * (centerX(r2) - centerX(r1)) + (centerY(r2) - centerY(r1)) * (centerY(r2) - centerY(r1)));
}

double DetectPlate::angleBetweenRect(Rect r1, Rect r2)
{	
	return atan((float) abs(r1.y - r2.y) / (float) abs(r1.x - r2.x));
	return 0.0;
}

double DetectPlate::diagonalSize(Rect r) {
	return sqrt(r.x * r.x + r.y * r.y);
}

vector<vector<Rect>> DetectPlate::splitRect(vector<Rect> R)
{	
	//từ  1 nhóm contours, phân thành các nhóm kề nhau.
	vector<bool> mark;
	vector<vector<Rect>> result;
	int size = R.size();
	for (int i = 0; i < size; i++)
		mark.push_back(false);
	std::vector<Rect> tempResult;
	for (int i = 0; i < size; i++) {
		tempResult.clear();
		for (int j = 0; j < size; j++) {
			if (!mark[j]) {
				Rect possibleChar = R[j];
				double DistanceBetweenChars = distanceBetweenRect(possibleChar, R[i]);
				double AngleBetweenChars = angleBetweenRect(possibleChar, R[i]);
				double ChangeInArea = (double)abs(possibleChar.area() - R[i].area()) / (double)possibleChar.area();
				double ChangeInWidth = (double)abs(possibleChar.width - possibleChar.width) / (double)possibleChar.width;
				double ChangeInHeight = (double)abs(possibleChar.height - possibleChar.height) / (double)possibleChar.height;

				// check if chars match
				if (DistanceBetweenChars < (diagonalSize(possibleChar) * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
					AngleBetweenChars < MAX_ANGLE_BETWEEN_CHARS &&
					ChangeInArea < MAX_CHANGE_IN_AREA &&
					ChangeInWidth < MAX_CHANGE_IN_WIDTH &&
					ChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
					tempResult.push_back(possibleChar);
					mark[j] = true;
				}
			}
		}
		if (tempResult.size() > 0)
			result.push_back(tempResult);
	}
	return result;
}



vector<Rect> DetectPlate::groupRect(Rect firstRect, vector<Rect>& inputRect)
{
	vector<Rect> result;

	for (vector<Rect>::iterator possibleInGroup = inputRect.begin(); possibleInGroup != inputRect.end(); possibleInGroup++) {
		if (*possibleInGroup == firstRect) {
			continue;
		}
		double dblDistanceBetweenRect = distanceBetweenRect(firstRect, (*possibleInGroup));
		double dblAngleBetweenRect = angleBetweenRect(firstRect, (*possibleInGroup));
		double dblChangeInArea = (double)abs(firstRect.area() - (*possibleInGroup).area()) / (double)(*possibleInGroup).area();
		double dblChangeInWidth = (double)abs(firstRect.width - (*possibleInGroup).width) / (double)(*possibleInGroup).width;
		double dblChangeInHeight = (double)abs(firstRect.height - (*possibleInGroup).height) / (double)(*possibleInGroup).height;

		// check if chars match
		if (dblDistanceBetweenRect < MAX_DIAG_SIZE_MULTIPLE_AWAY &&
			dblAngleBetweenRect < MAX_ANGLE_BETWEEN_CHARS &&
			dblChangeInArea < MAX_CHANGE_IN_AREA &&
			dblChangeInWidth < MAX_CHANGE_IN_WIDTH &&
			dblChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
			result.push_back(*possibleInGroup); 
			inputRect.erase(possibleInGroup);
		}
	}

	return result;

}

vector<Mat> DetectPlate::findPlate(Mat srcImg)
{
	vector <Mat> plate;
	vector<vector<Point>> contours;
	vector<Rect> charContours;
	vector<Rect> R;
	Mat temp = srcImg.clone();
	temp = Preprocess::convertToGray(temp);
	//srcImg = Preprocess::gBlur(srcImg);
	temp = Preprocess::thresholdImage(temp, 19, 9);
	//imshow("srcImg", srcImg);
	findContours(temp, contours, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*for (int i = 0; i < contours.size(); i++) {
		drawContours(temp, contours, i, Scalar(0, 0, 255), 1, 8, noArray(), INT_MAX, Point(0, 0));
	}*/
	int k = 0;
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		float w = (float) r.width, h = (float)r.height, s = w * h;
		float ratio = ((w == 0) ? 0 : (h == 0) ? 0 : w / h);
		Mat temp1(temp, r);
		float ratiobw = (float)countNonZero(temp1) / s;

		if (s > 100 && s < 300 && 0.25 < ratio && ratio < 1.1) {
			rectangle(temp, r, Scalar(255, 0, 255), 1, 8, 0);
			Mat temp2(temp, r);
			charContours.push_back(r);
		}
		temp1.~Mat();
	}
	vector<vector<Rect>> groupOfRect = splitRect(charContours);
	for (auto charContours : groupOfRect) {
		Mat imgContours = Mat(srcImg.size(), CV_8UC3, Scalar(0, 0, 0));
		drawContours(imgContours, charContours, -1, Scalar(255, 255, 255));
		sort(charContours.begin(), charContours.end(), sortContours);
		imshow("2b", imgContours);

		double dblPlateCenterX = (double)(centerX(charContours[0])) + centerX(charContours[charContours.size() - 1]) / 2.0;
		double dblPlateCenterY = (double)(centerY(charContours[0])) + centerY(charContours[charContours.size() - 1]) / 2.0;
		cv::Point2d p2dPlateCenter(dblPlateCenterX, dblPlateCenterY);

		// calculate plate width and height
		int intPlateWidth = (int)(charContours[charContours.size() - 1].x + charContours[charContours.size() - 1].width - charContours[0].x * PLATE_WIDTH_PADDING_FACTOR);

		double intTotalOfCharHeights = 0;

		for (auto matchingChar : charContours) {
			intTotalOfCharHeights = intTotalOfCharHeights + boundingRect(matchingChar).height;
		}

		double dblAverageCharHeight = (double)intTotalOfCharHeights / charContours.size();

		int intPlateHeight = (int)(dblAverageCharHeight * 1.5);

		// calculate correction angle of plate region
		double dblOpposite = centerY(charContours[charContours.size() - 1]) - centerY(charContours[0]);
		double dblHypotenuse = distanceBetweenRect(charContours[0], charContours[charContours.size() - 1]);
		double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);
		double dblCorrectionAngleInDeg = dblCorrectionAngleInRad * (180.0 / CV_PI);

		// assign rotated rect member variable of possible plate
		RotatedRect rrLocationOfPlateInScene;
		rrLocationOfPlateInScene = cv::RotatedRect(p2dPlateCenter, cv::Size2f((float)intPlateWidth, (float)intPlateHeight), (float)dblCorrectionAngleInDeg);

		cv::Mat rotationMatrix;             // final steps are to perform the actual rotation
		cv::Mat imgRotated;
		cv::Mat imgCropped;

		rotationMatrix = cv::getRotationMatrix2D(p2dPlateCenter, dblCorrectionAngleInDeg, 1.0);         // get the rotation matrix for our calculated correction angle

		cv::warpAffine(srcImg, imgRotated, rotationMatrix, srcImg.size());            // rotate the entire image

		imshow("rotated", imgRotated);
	}
	// crop out the actual plate portion of the rotated image
/*cv::getRectSubPix(imgRotated, rrLocationOfPlateInScene.size, rrLocationOfPlateInScene.center, imgCropped);


imshow("cropped", imgCropped);*/


	waitKey(0);
	return imgCropped;
}

DetectPlate::DetectPlate()
{
}

DetectPlate::~DetectPlate()
{
}



