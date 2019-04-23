#include "Preprocess.h"



Mat Preprocess::convertToGray(Mat srcImg)
{
	Mat grayImg;
	cvtColor(srcImg, grayImg, COLOR_BGR2GRAY);
	return grayImg;
}

Mat Preprocess::simpleThreshold(Mat srcImg, int threshValue)
{	
	Mat thresholdImg;
	cv::threshold(srcImg, thresholdImg, threshValue, 255, THRESH_BINARY);
	return thresholdImg;
}

Mat Preprocess::adaptiveThreshold(Mat grayImg, int blockSize, double C)
{
	Mat thresholdImg;
	GaussianBlur(grayImg, grayImg, Size(5, 5), 0);
	cv::adaptiveThreshold(grayImg, thresholdImg, 255.0, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, blockSize, C);
	return thresholdImg;
}

Mat Preprocess::gBlur(Mat grayImg)
{
	GaussianBlur(grayImg, grayImg, Size(5,5), 0);
	return grayImg;
}

Preprocess::Preprocess()
{
}


Preprocess::~Preprocess()
{
}
