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
	DetectPlate();
	virtual ~DetectPlate();
};

