#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <filesystem>
#include <iostream>
#include "Preprocess.h"
#include "DetectPlate.h"
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
	static Ptr<KNearest> knn;
	static SVM svm;
	void trainKNN(string path);
	void trainKNNUsingXML();
	void trainSVM(string path);
	char detectKNN(Mat srcImg);
	char detectSVM(Mat srcImg);
	static bool checkIfPossibleChar(Rect r, Mat thresholdImg);
	static vector<Mat> splitChar(vector<Mat> plate, Mat sourceImg);
	DetectChar();
	~DetectChar();
};

