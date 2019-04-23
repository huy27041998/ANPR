#pragma once
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
using namespace std;
using namespace cv;
const double MIN_DIAG_SIZE_MULTIPLE_AWAY = 0.3;
const double MAX_DIAG_SIZE_MULTIPLE_AWAY = 5.0;
const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;
const double MAX_CHANGE_IN_AREA = 0.5;
const double MAX_CHANGE_IN_WIDTH = 0.8;
const double MAX_CHANGE_IN_HEIGHT = 0.2;
const double MAX_ANGLE_BETWEEN_CHARS = 12.0;
const double MAX_DISTANCE_BETWEEN_CHARS = 50;
const double MAX_CHAR_WIDTH = 60;
const double MAX_CHAR_AREA = 1000;
class DetectPlate
{
public:
	vector<Mat> findPlate(Mat srcImg);  //Tìm các vùng có khả năng là biển số xe
	static bool sortRectX(const Rect &r1, const Rect &r2); //Sắp xếp lại đường viền theo phương X
	static bool sortRectY(const Rect &r1, const Rect &r2); //Sắp xếp lại đường viền theo phương Y
	static bool sortRectXY(const Rect &r1, const Rect &r2);
	int centerX(Rect r);
	int centerY(Rect r);
	double distanceBetweenRect(Rect r1, Rect r2);
	double angleBetweenRect(Rect r1, Rect r2);
	double diagonalSize(Rect r);
	bool checkIfRectPossibleChar(Rect r, Mat thresholdImg);
	void splitRect(vector<Rect> &R, vector<vector<Rect>> &result, vector<Rect> &tempResult, int startRect);
	DetectPlate();
	virtual ~DetectPlate();
};

