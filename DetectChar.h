#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
using namespace std;
using namespace cv;
class DetectChar
{
public:
	vector<Mat> splitChar(Mat thresholdImg, Mat sourceImg);
	DetectChar();
	~DetectChar();
};

