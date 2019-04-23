#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
class Preprocess
{
public:
	static Mat convertToGray(Mat srcImg);
	static Mat simpleThreshold(Mat srcImg, int threshValue);
	static Mat adaptiveThreshold(Mat grayImg, int blockSize, double C);
	static Mat gBlur(Mat grayImg);
	Preprocess();
	virtual ~Preprocess();
};

