#include "DetectPlate.h"

bool DetectPlate::checkIfRectPossibleChar(Rect r, Mat thresholdImg) {
	int s = r.area();
	int w = r.width;
	Mat temp(thresholdImg, r);
	float ratio = (float)countNonZero(temp) / s;
	temp.~Mat();
	return (200 < s && s < MAX_CHAR_AREA  && w < MAX_CHAR_WIDTH);
	return true;
}

bool DetectPlate::sortRectX(const Rect &r1, const Rect &r2)
{
	return (r1.x < r2.x);
}

bool DetectPlate::sortRectY(const Rect &r1, const Rect &r2)
{	
	return (r1.y < r2.y && abs(r1.y - r2.y) > 2);
}
bool DetectPlate::sortRectXY(const Rect &r1, const Rect &r2) {
	return r1.x * r1.x + r1.y * r1.y < r2.x * r2.x + r2.y * r2.y;
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
	return atan((float) abs(centerY(r1) - centerY(r2)) / (float) abs(centerX(r1) - centerX(r2)));
	return 0.0;
}

double DetectPlate::diagonalSize(Rect r) {
	return sqrt(r.x * r.x + r.y * r.y);
}

void DetectPlate::splitRect(vector<Rect> &R, vector<vector<Rect>> &result, vector<Rect> &tempResult, int startRect)
{	
	//từ  1 nhóm contours, phân thành các nhóm kề nhau.
	if (R.size() == 1)
		return;
	Rect start = R[startRect];
	R.erase(R.begin() + startRect);
	int size = R.size();
	for (int i = 0; i < size; i++) {
		if (i >= R.size()) return;
		Rect possibleChar = R[i];
		double DistanceBetweenChars = distanceBetweenRect(possibleChar, start);
		double AngleBetweenChars = angleBetweenRect(possibleChar, start);
		double ChangeInArea = (double)abs(possibleChar.area() - start.area()) / (double)possibleChar.area();
		double ChangeInWidth = (double)abs(possibleChar.width - start.width) / (double)possibleChar.width;
		double ChangeInHeight = (double)abs(possibleChar.height - start.height) / (double)possibleChar.height;

		// check if chars match
		if (DistanceBetweenChars < MAX_DISTANCE_BETWEEN_CHARS &&
			AngleBetweenChars < MAX_ANGLE_BETWEEN_CHARS &&
			ChangeInArea < MAX_CHANGE_IN_AREA &&
			ChangeInWidth < MAX_CHANGE_IN_WIDTH &&
			ChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
			tempResult.push_back(possibleChar);
			splitRect(R, result, tempResult, i);
		}	
	}
	result.push_back(tempResult);
	tempResult.clear();
	if (R.size() > 0) {
		tempResult.push_back(R[0]);
		splitRect(R, result, tempResult, 0);
	}
}

vector<Mat> DetectPlate::findPlate(Mat srcImg)
{
	vector <Mat> plate;
	vector<vector<Point>> contours;
	vector<Rect> charContours;
	Mat thresholdImg = srcImg.clone();
	thresholdImg = Preprocess::convertToGray(thresholdImg);
	//srcImg = Preprocess::gBlur(srcImg); //xóa nhiều làm trơn nhiễu gauss.
	thresholdImg = Preprocess::simpleThreshold(thresholdImg, 127);
	//threshold(thresholdImg, thresholdImg, 80, 255, THRESH_BINARY_INV);
	findContours(thresholdImg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*for (int i = 0; i < contours.size(); i++) {
		drawContours(temp, contours, i, Scalar(0, 0, 255), 1, 8, noArray(), INT_MAX, Point(0, 0));
	}*/
	Mat display = srcImg.clone();
	int k = 0;
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		if (checkIfRectPossibleChar(r, thresholdImg)) {
			rectangle(display, r, Scalar(255, 0, 255), 1, 8, 0); //vẽ hình ra ảnh
			charContours.push_back(r);
		}
	}
	imshow("display", display);
	waitKey(0);
	//Sắp xếp lại đường viền.
	sort(charContours.begin(), charContours.end(), sortRectXY);
	vector <vector<Rect>> groupOfRect;
	vector<Rect> tempResult;
	tempResult.push_back(charContours[0]);
	splitRect(charContours, groupOfRect, tempResult, 0);
	for (auto group : groupOfRect) {
		if (group.size() > 3) {
			sort(group.begin(), group.end(), sortRectX);
			sort(group.begin(), group.end(), sortRectY);
			int size = group.size();
			double plateCenterX = (double)(centerX(group[0]) + centerX(group[size - 1])) / 2.0;
			double plateCenterY = (double)(centerY(group[0]) + centerY(group[size - 1])) / 2.0;
			Point2d plateCenter = Point2d(plateCenterX, plateCenterY);
			double plateWidth =  PLATE_WIDTH_PADDING_FACTOR * distanceBetweenRect(group[0], group[size - 1]);
			double plateHeight = 0;
			cout << endl << plateCenter << endl;
			plateHeight = plateHeight / size * PLATE_HEIGHT_PADDING_FACTOR;
			double plateAngle = angleBetweenRect(group[0], group[size - 1]);
			RotatedRect r = RotatedRect(plateCenter, Size2f((float)plateWidth, (float)plateHeight), (float)plateAngle);
			//diplay Rotated Rect
			//{
			//	cv::Point2f vertices2f[4];
			//	r.points(vertices2f);
			//	Point vertices[4];
			//	for (int i = 0; i < 4; ++i) {
			//		vertices[i] = vertices2f[i];
			//	}
			//	cv::fillConvexPoly(display, vertices, 4, Scalar(0, 0, 255));
			//	imshow("display", display);
			//	waitKey(0); 
			//}
			Mat imgCroped;
			Mat temp = srcImg.clone();
			Mat rotationMatrix = getRotationMatrix2D(plateCenter, plateAngle, 1.0) * 180 / CV_PI;
			warpAffine(temp, imgCroped, rotationMatrix, temp.size());
			imshow("temp", temp);
			waitKey(0);
			temp = Preprocess::convertToGray(temp);
			getRectSubPix(temp, Size2f((float)plateWidth, (float)plateHeight), plateCenter, imgCroped);
			plate.push_back(imgCroped);
			temp.~Mat();
			rotationMatrix.~Mat();
		}
	}
	cout << plate.size() << " found.";
	return plate;
}

DetectPlate::DetectPlate()
{
}

DetectPlate::~DetectPlate()
{
}



