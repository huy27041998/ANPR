#include "DetectPlate.h"



bool DetectPlate::sortContours(vector<Point> contours1, vector<Point> contours2)
{
	Rect r1 = boundingRect(contours1), r2 = boundingRect(contours2);
	return (r1.x < r2.x);
}

int DetectPlate::centerX(Rect r)
{
	return (r.x + (float) r.width / 2);
}

int DetectPlate::centerY(Rect r)
{
	return (r.y + (float)r.height / 2);
}
double DetectPlate::distanceBetweenChars(Rect r1, Rect r2) {
	return sqrt((centerX(r2) - centerX(r1)) * (centerX(r2) - centerX(r1)) + (centerY(r2) - centerY(r1)) * (centerY(r2) - centerY(r1)));
}
vector<Mat> DetectPlate::findPlate(Mat srcImg)
{
	vector <Mat> plate;
	vector<vector<Point>> contours;
	vector<vector<Point>> charContours;
	vector<Rect> R;
	Mat temp = srcImg.clone();
	temp = Preprocess::convertToGray(temp);
	//srcImg = Preprocess::gBlur(srcImg);
	temp = Preprocess::thresholdImage(temp, 19, 9);
	//imshow("srcImg", srcImg);
	findContours(temp, contours, RETR_LIST ,CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*for (int i = 0; i < contours.size(); i++) {
		drawContours(temp, contours, i, Scalar(0, 0, 255), 1, 8, noArray(), INT_MAX, Point(0, 0));
	}*/
	int k = 0;
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		float w = r.width, h = r.height, s = w * h;
		float ratio = ((w == 0) ? 0 : (h == 0) ? 0 : w / h);
		Mat temp1(temp, r);
		float ratiobw = (float)countNonZero(temp1) / s;
		
		if (s > 100 && s < 300 && 0.25 < ratio && ratio < 1.1) {
			rectangle(temp, r, Scalar(255, 0, 255), 1, 8, 0);
			Mat temp2(temp, r);
			charContours.push_back(contours[i]);
		}
		temp1.~Mat();
	}

	Mat imgContours = Mat(srcImg.size(), CV_8UC3, Scalar(0, 0, 0));
	drawContours(imgContours, charContours, -1, Scalar(255, 255, 255));
	sort(charContours.begin(), charContours.end(), sortContours);
	imshow("2b", imgContours);

	double dblPlateCenterX = (double)(centerX(boundingRect(charContours[0])) + centerX(boundingRect(charContours[charContours.size() - 1]))) / 2.0;
	double dblPlateCenterY = (double)(centerY(boundingRect(charContours[0])) + centerY(boundingRect(charContours[charContours.size() - 1]))) / 2.0;
	cv::Point2d p2dPlateCenter(dblPlateCenterX, dblPlateCenterY);

	// calculate plate width and height
	int intPlateWidth = (int)((boundingRect(charContours[charContours.size() - 1]).x + boundingRect(charContours[charContours.size() - 1]).width - boundingRect(charContours[0]).x) * 1.3);

	double intTotalOfCharHeights = 0;

	for (auto matchingChar : charContours) {
		intTotalOfCharHeights = intTotalOfCharHeights + boundingRect(matchingChar).height;
	}

	double dblAverageCharHeight = (double)intTotalOfCharHeights / charContours.size();

	int intPlateHeight = (int)(dblAverageCharHeight * 1.5);

	// calculate correction angle of plate region
	double dblOpposite = centerY(boundingRect(charContours[charContours.size() - 1])) - centerY(boundingRect(charContours[0]));
	double dblHypotenuse = distanceBetweenChars(boundingRect(charContours[0]), boundingRect(charContours[charContours.size() - 1]));
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
