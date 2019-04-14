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
class DetectPlate
{
public:
	vector<Mat> findPlate(Mat srcImg);
	static bool sortContours(vector<Point> contours1, vector<Point> contours2);
	int centerX(Rect r);
	int centerY(Rect r);
	double distanceBetweenRect(Rect r1, Rect r2);
	double angleBetweenRect(Rect r1, Rect r2);
	double diagonalSize(Rect r);
	vector<vector<Rect>> splitRect(vector<Rect> rect);
	vector<Rect> groupRect(Rect firstRect, vector<Rect> &inputRect);
	vector<vector<Point>>contours;
	DetectPlate();
	virtual ~DetectPlate();
};

