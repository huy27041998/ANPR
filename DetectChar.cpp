#include "DetectChar.h"

bool DetectChar::checkIfPossibleChar(Rect r, Mat thresholdImg) {
	int s = r.area();
	int w = r.width;
	Mat temp(thresholdImg, r);
	float ratio = (float)countNonZero(temp) / s;
	temp.~Mat();
	return (ratio > 0.3 && ratio < 0.7 && 200 < s && r.width > 10 && r.width < 27 && r.height < 60 &&  s < MAX_CHAR_AREA  && w < MAX_CHAR_WIDTH);
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
			temp = Preprocess::convertToGray(temp);
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
			temp = Preprocess::convertToGray(temp);
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
	}
	this->svm = SVM::create();
	this->svm->setType(SVM::C_SVC);
	this->svm->setKernel(SVM::LINEAR);
	this->svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 10, 1e-3));
	this->svm->train(traindata, ROW_SAMPLE, classification);
}

void DetectChar::trainKNNUsingXML()
{
	Mat matClassificationInts;
	FileStorage fsClassifications("classifications.xml", FileStorage::READ);

	if (fsClassifications.isOpened() == false) {
		std::cout << "error, unable to open training classifications file, exiting program\n\n";
		return;
	}

	fsClassifications["classifications"] >> matClassificationInts;
	fsClassifications.release();


	Mat matTrainingImagesAsFlattenedFloats;

	FileStorage fsTrainingImages("images.xml", FileStorage::READ);

	if (fsTrainingImages.isOpened() == false) {
		std::cout << "error, unable to open training images file, exiting program\n\n";
		return;
	}

	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
	fsTrainingImages.release();


	this->knn = KNearest::create();
	this->knn->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);

}


char DetectChar::detectKNN(Mat srcImg)
{
	Mat temp = srcImg.clone();
	//temp = Preprocess::convertToGray(temp);
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
	resize(temp, temp, Size(30, 30));
	temp.convertTo(temp, CV_32FC1);
	float r = this->svm->predict(temp.reshape(1, 1));
	return (char)(int)r;
}

vector<Mat> DetectChar::splitChar(Mat plate)
{
	vector<Mat> character;
	vector<vector<Point>> contours;
	Mat thresholdImg = Preprocess::adaptiveThreshold(plate, 19, 9);
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
			R.push_back(r);
		}
	}
	imshow("Possible Char", display);
	waitKey(0);
	display.~Mat();
	if (R.size() == 0) {
		cout << "Khong tim thay chu cai." << endl;
		waitKey(0);
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

DetectChar::DetectChar()
{

}

DetectChar::~DetectChar()
{
}
