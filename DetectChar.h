#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/utility.hpp"
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
	Ptr<KNearest> knn;
	Ptr<KNearest> knn1;
	Ptr<SVM> svm;
	Ptr<SVM> svm1;
	void trainKNN(string path);
	void trainSVM(string path);
	void trainSVM1(string path);
	char detectKNN(Mat srcImg);
	char detectSVM(Mat srcImg);
	char detectSVM1(Mat srcImg);
	string tesseractPredict(Mat srcImg);
	static bool checkIfPossibleChar(Rect r, Mat thresholdImg);
	static vector<Mat> splitChar(Mat plate);
	string detectSVMString(vector<Mat> character);
	string detectKNNString(vector<Mat> character);
	string detectSVM1String(vector<Mat> character);
	vector<float> calculate_feature(Mat src);
	DetectChar();
	~DetectChar();
};

