#pragma once
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
using namespace std;
using namespace cv;
class DetectPlate
{
public:
	vector<Mat> findPlate(Mat srcImg);
	static bool sortContours(vector<Point> contours1, vector<Point> contours2);
	int centerX(Rect r);
	int centerY(Rect r);
	double distanceBetweenChars(Rect r1, Rect r2);
	DetectPlate();
	virtual ~DetectPlate();
};

