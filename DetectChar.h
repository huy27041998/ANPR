#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <filesystem>
#include <iostream>
#include "Preprocess.h"
using namespace std;
using namespace cv;
using namespace ml;
namespace fs = std::experimental::filesystem;
const int MIN_AREA = 80;
const int MAX_AREA = 100;
const float MIN_RATIO = 0.5;
const float MAX_RATIO = 1;
class DetectChar
{
public:
	Ptr<KNearest> knn;
	void train(string path);
	void train1();
	char detect(Mat srcImg);
	static vector<Mat> splitChar(Mat thresholdImg, Mat sourceImg);
	DetectChar();
	~DetectChar();
};

