#pragma once
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
using namespace std;
using namespace cv;
class DetectPlate1
{
public:
	vector<Mat> findPlate(Mat srcImg);
	static bool sortContours(vector<Point> contours1, vector<Point> contours2);
	bool verifySizes(RotatedRect candidate);
	DetectPlate1();
	virtual ~DetectPlate1();
};

