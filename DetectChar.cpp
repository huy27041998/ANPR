#include "DetectChar.h"

bool DetectChar::checkIfPossibleChar(Rect r, Mat thresholdImg) {
	int s = r.area();
	int w = r.width;
	Mat temp(thresholdImg, r);
	float ratio = (float)countNonZero(temp) / s;
	temp.~Mat();
	return (ratio > 0.3 && ratio < 0.7 && 300 < s && r.width < 27 && r.height > 24 && r.height < 60 &&  s < MAX_CHAR_AREA  && w < MAX_CHAR_WIDTH);
	//return true;
}

void DetectChar::trainKNN(string path)
{
	Mat classification;
	Mat traindata;
	int i = '0';
	for (auto &p : fs::directory_iterator(path)) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			if (temp.channels() != 1) {
				temp = Preprocess::convertToGray(temp);
			}
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32FC1);
			temp = temp.reshape(1, 1);
			traindata.push_back(temp);
			classification.push_back(i);
		}
		if (i != int('9'))
			i++;
		else i = int('A');
	}
	knn = KNearest::create();
	knn->train(traindata, ROW_SAMPLE, classification);

}

void DetectChar::trainSVM(string path)
{
	Mat classification;
	Mat traindata;
	int i = '0';
	for (auto &p : fs::directory_iterator(path)) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			if (temp.channels() != 1) {
				temp = Preprocess::convertToGray(temp);
			}
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32FC1);
			temp = temp.reshape(1, 1);
			traindata.push_back(temp);
			classification.push_back(i);
		}
		if (i != (int) '9')
			i++;
		else
			i = (int) 'A';
		if (i == (int)'I')
			i = (int) 'I';
	}
	this->svm = SVM::create();
	this->svm->setType(SVM::C_SVC);
	this->svm->setKernel(SVM::LINEAR);
	this->svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 10, 1e-3));
	this->svm->train(traindata, ROW_SAMPLE, classification);
}

vector<float> DetectChar::calculate_feature(Mat src)
{
	Mat img;
	if (src.channels() != 1)
	{
		img = Preprocess::convertToGray(src);
		threshold(img, img, 100, 255, THRESH_BINARY);
	}
	else
	{
		threshold(src, img, 100, 255, THRESH_BINARY);
	}

	vector<float> r;
	resize(img, img, Size(40, 40));
	int h = img.rows / 15;
	int w = img.cols / 15;
	int S = countNonZero(img);
	int T = img.cols * img.rows;
	for (int i = 0; i < img.rows; i += h)
	{
		for (int j = 0; j < img.cols; j += w)
		{
			Mat cell = img(Rect(i, j, h, w));
			int s = countNonZero(cell);
			float f = (float)s / S;
			r.push_back(f);
		}
	}
	return r;
}

char DetectChar::detectKNN(Mat srcImg)
{
	Mat temp = srcImg.clone();
	if (srcImg.channels() != 1)
		temp = Preprocess::convertToGray(temp);
	resize(temp, temp, Size(30, 30));
	temp.convertTo(temp, CV_32F);
	Mat detect(30, 30, CV_32F);
	knn->findNearest(temp.reshape(1, 1), 1, detect);
	float fltCurrentChar = (float)detect.at<float>(0, 0);
	return char(int(fltCurrentChar));

}

char DetectChar::detectSVM(Mat srcImg)
{
	Mat temp = srcImg.clone();
	if (srcImg.channels() != 1)
		temp = Preprocess::convertToGray(temp);
	resize(temp, temp, Size(30, 30));
	temp.convertTo(temp, CV_32FC1);
	float r = this->svm->predict(temp.reshape(1, 1));
	return (char)(int)r;
}

string DetectChar::tesseractPredict(Mat srcImg)
{
	return string();
}

vector<Mat> DetectChar::splitChar(Mat plate)
{
	vector<Mat> character;
	vector<vector<Point>> contours;
	Mat grayImg = Preprocess::convertToGray(plate);
	Mat thresholdImg = Preprocess::adaptiveThreshold(grayImg, THRESH_BINARY_INV, 19, 9);
	findContours(thresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//Vẽ viền
	Mat display = plate.clone();
	/*for (int i = 0; i < contours.size(); i++)
		drawContours(display, contours, i, Scalar(0, 255, 0), 1, 8, noArray(), INT_MAX, Point(0, 0));
	imshow("display", display);*/
	waitKey(0);
	vector <Rect> R; //Lưu tọa độ của bao viền
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		if (checkIfPossibleChar(r, thresholdImg)) {
			rectangle(display, r, Scalar(0, 0, 255), 1, 8, 0);
			//cout << r.width << " " << r.height << endl;
			R.push_back(r);
		}
	}
	imshow("Possible Char", display);
	waitKey(0);
	display.~Mat();
	if (R.size() == 0) {
		return character;

	}
	//Sắp xếp 
	//Xếp theo hàng dọc
	for (int i = 0; i < R.size() - 1; i++)
		for (int j = i + 1; j < R.size(); j++) {
			if (R[i].y > R[j].y)
				swap(R[i], R[j]);
		}
	//Xếp theo hàng ngang
	for (int i = 0; i < R.size() - 1; i++)
		for (int j = i + 1; j < R.size(); j++) {
			if (R[i].x > R[j].x && fabs(R[i].y - R[j].y) < 2)
				swap(R[i], R[j]);
		}
	for (int i = 0; i < R.size(); i++) {
		Mat temp(plate, R[i]);
		character.push_back(temp);
		temp.~Mat();
		
	}
	return character;
}

string DetectChar::detectSVMString(vector<Mat> character)
{
	string s = "";
	for (int i = 0; i < character.size(); i++) {
		s += detectSVM(character[i]);
	}
	return s;
}

string DetectChar::detectKNNString(vector<Mat> character)
{
	string s = "";
	for (int i = 0; i < character.size(); i++) {
		s += detectKNN(character[i]);
	}
	return s;
}

void DetectChar::trainSVM1(string path)
{
	Mat classification;
	Mat traindata;
	int i = '0';
	for (auto &p : fs::directory_iterator(path)) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat train;
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			vector<float> r = calculate_feature(temp);
			train.push_back(r);
			transpose(train, train);
			traindata.push_back(train);
			classification.push_back(i);
			train.~Mat();
		}
		if (i != (int) '9')
			i++;
		else
			i = (int) 'A';
		if (i == (int)'I')
			i = (int) 'I';
	}
	this->svm1 = SVM::create();
	this->svm1->setType(SVM::C_SVC);
	this->svm1->setKernel(SVM::LINEAR);
	this->svm1->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 11, 1e-6));
	this->svm1->train(traindata, ROW_SAMPLE, classification);
	//this->knn1 = KNearest::create();
	//this->knn1->train(traindata, ROW_SAMPLE, classification);
}

char DetectChar::detectSVM1(Mat srcImg)
{
	Mat temp = srcImg.clone();
	vector<float> r = calculate_feature(temp);
	Mat predict(0, 0, CV_32FC1);
	predict.push_back(r);
	transpose(predict, predict);
	predict.convertTo(predict, CV_32F);
	float result = this->svm1->predict(predict);
	//Mat detect(0, 0, CV_32F);
	//this->knn1->findNearest(predict, 5, detect);
	//float result = detect.at<float>(0, 0);
	//detect.~Mat();
	predict.~Mat();
	return (char)(int)result;
}

string DetectChar::detectSVM1String(vector<Mat> character)
{
	string s = "";
	for (int i = 0; i < character.size(); i++) {
		s += detectSVM1(character[i]);
	}
	return s;
}

DetectChar::DetectChar()
{

}

DetectChar::~DetectChar()
{
}
