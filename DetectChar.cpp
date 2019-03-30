#include "DetectChar.h"



void DetectChar::train(string path)
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
void DetectChar::train1()
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
char DetectChar::detect(Mat srcImg)
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
vector<Mat> DetectChar::splitChar(Mat thresholdImg, Mat sourceImg)
{
	int w = sourceImg.cols, h = sourceImg.rows;
	cout << w << " " << h << endl;
	vector<Mat> character;
	vector<vector<Point>> contours;
	Mat temp1 = sourceImg;
	//Tìm viền
	findContours(thresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
	//Vẽ viền
	//for (int i = 0; i < contours.size(); i++) 
		//drawContours(temp1, contours, i, Scalar(0,0,255), 1, 8, noArray(), INT_MAX, Point(0, 0));	
	vector <Rect> R; //Lưu tọa độ của bao viền
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		int wr = r.width, hr = r.height, s = wr * hr;
		cout << wr << " " << hr << endl;
		float ratio = ((wr == 0) ? 0 : (hr == 0) ? 0 : (float)wr / hr);
		Mat temp2(thresholdImg, r);
		if (s != 0) 
			float ratiobw = (float)countNonZero(temp2) / s;
		if (wr > w * 0.07 && wr < w * 0.17 && hr > h * 0.4 && hr < 0.5 * h) {
			//rectangle(temp1, r, Scalar(0, 0, 255), 1, 8, 0);
			R.push_back(r);
		}
		}
	imshow("char", temp1);
	temp1.~Mat();	
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
	return character;
}

DetectChar::DetectChar()
{
}


DetectChar::~DetectChar()
{
}
