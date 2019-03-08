#include "Preprocess.h"



Mat Preprocess::convertToGray(Mat srcImg)
{
	Mat grayImg;
	cvtColor(srcImg, grayImg, COLOR_BGR2GRAY);
	return grayImg;
}

Mat Preprocess::thresholdImage(Mat grayImg)
{
	Mat thresholdImg;
	//GaussianBlur(grayImg, grayImg, Size(5,5), 0);
	adaptiveThreshold(grayImg, thresholdImg, 255.0, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 9, 19);
	return thresholdImg;
}

Preprocess::Preprocess()
{
}


Preprocess::~Preprocess()
{
}
