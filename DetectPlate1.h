#pragma once
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
using namespace std;
using namespace cv;
const int MAX_PLATE_WIDTH = 240;
const int MIN_PLATE_WIDTH = 110;
const int MAX_PLATE_HEIGTH = 120;
const int MIN_PLATE_HEIGTH = 35;
const double MAX_PLATE_RATIO = 0.8;
const double MIN_PLATE_RATIO = 0.3;
class DetectPlate1
{
public:
	vector<Mat> findPlate(Mat srcImg);
	bool verifySizes(RotatedRect candidate, Mat grayImg);
	void draw(RotatedRect rect, Mat &srcImg);
	DetectPlate1();
	virtual ~DetectPlate1();
};

