#include "DetectChar.h"

bool DetectChar::checkIfPossibleChar(Rect r, Mat thresholdImg) {
	int s = r.area();
	int w = r.width;
	Mat temp(thresholdImg, r);
	float ratio = (float)countNonZero(temp) / s;
	temp.~Mat();
	return (200 < s && s < MAX_CHAR_AREA  && w < MAX_CHAR_WIDTH);
	return true;
}

void DetectChar::trainKNN(string path)
{
	Mat classification;
	Mat traindata;
	int i = '0';
	for (auto &p : fs::directory_iterator(path)) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32FC1);
			temp = temp.reshape(1, 1);
			traindata.push_back(temp);
			classification.push_back(i);
		}
		if (i != int('9'))
			i++;
		else i = int('a');
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
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32FC1);
			temp = temp.reshape(1, 1);
			traindata.push_back(temp);
			classification.push_back(i);
		}
		if (i != int('9'))
			i++;
		else i = int('a');
	}
	this->svm.trainAuto(traindata, ROW_SAMPLE, classification);
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
	resize(temp, temp, Size(20, 30));
	temp.convertTo(temp, CV_32FC1);
	temp = temp.reshape(1, 1);
	Mat detect(0, 0, CV_32FC1);
	knn->findNearest(temp, 1, detect);
	float fltCurrentChar = (float)detect.at<float>(0, 0);
	return char(int(fltCurrentChar));
	
}

char DetectChar::detectSVM(Mat srcImg)
{
	Mat temp = srcImg.clone();
	resize(temp, temp, Size(20, 30));
	temp.convertTo(temp, CV_32FC1);
	temp = temp.reshape(1, 1);
	float r = this->svm.predict(temp);
	char c = '*';
	int ri = (int) r;
	if (ri >= 0 && ri <= 9)
		c = (char)(ri + 48); //ma ascii 0 = 48
	if (ri >= 10 && ri < 18)
		c = (char)(ri + 55); //ma accii A = 5, --> tu A-H
	if (ri >= 18 && ri < 22)
		c = (char)(ri + 55 + 2); //K-N, bo I,J
	if (ri == 22) c = 'P';
	if (ri == 23) c = 'S';
	if (c >= 24 && ri < 27)
		c = (char)(ri + 60); //T-V,  
	if (ri >= 27 && ri < 30)
		c = (char)(ri + 61); //X-Z
	return c;
}

vector<Mat> DetectChar::splitChar(vector<Mat> plate, Mat sourceImg)
{
	vector<Mat> character;
	for (auto possiblePlate : plate) {
		
		vector<vector<Point>> contours;
		Mat thresholdImg = Preprocess::simpleThreshold(possiblePlate, 127);
		//Tìm viền
		findContours(thresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		//Vẽ viền
		//for (int i = 0; i < contours.size(); i++) 
			//drawContours(temp1, contours, i, Scalar(0,0,255), 1, 8, noArray(), INT_MAX, Point(0, 0));	
		Mat display = sourceImg.clone();
		vector <Rect> R; //Lưu tọa độ của bao viền
		for (int i = 0; i < contours.size(); i++) {
			Rect r = boundingRect(contours[i]);
			if (checkIfPossibleChar(r, thresholdImg)) {
				//rectangle(, r, Scalar(0, 0, 255), 1, 8, 0);
				R.push_back(r);
			}
		}
		imshow("char", display);
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
				if (R[i].x > R[j].x && fabs(R[i].y - R[j].y) < 0.5)
					swap(R[i], R[j]);
			}
		for (int i = 0; i < R.size(); i++) {
			Mat temp(sourceImg, R[i]);
			character.push_back(temp);
			temp.~Mat();
		}
	}
	return character;
}

DetectChar::DetectChar()
{

}

DetectChar::~DetectChar()
{
}
