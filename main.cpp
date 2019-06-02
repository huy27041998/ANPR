#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
#include "DetectChar.h"
#include "GenData.h"
#include "DetectPlate.h"
#include "DetectPlate1.h"
#include <fstream>
using namespace std;
using namespace cv;


int main()
{
	Mat srcImg = imread("P1010052.jpg", IMREAD_UNCHANGED);
	//GenData gen("C:\\Users\\Huy\\Desktop\\Croatia.png", 250);
	if (srcImg.empty()) {
		cout << "Cannot find image";
		cin.get();
		return 0;
	}
	string result;
	imshow("Source Image", srcImg);
	waitKey(0);
	DetectPlate1 dp;
	DetectChar dc;
	//vector<vector<Point>> contours;
	//Mat thresholdImg = Preprocess::adaptiveThreshold(srcImg, 19, 9);
	//findContours(thresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//Vẽ viền
	//Mat display = srcImg.clone();
	//for (int i = 0; i < contours.size(); i++)
	//	drawContours(display, contours, i, Scalar(0, 255, 0), 1, 8, noArray(), INT_MAX, Point(0, 0));
	//imshow("display", display);
	//waitKey(0);
	//vector <Rect> R; //Lưu tọa độ của bao viền
	//for (int i = 0; i < contours.size(); i++) {
	//	Rect r = boundingRect(contours[i]);
	//	rectangle(display, r, Scalar(0, 0, 255), 1, 8, 0);
	//	if (dt.checkIfPossibleChar(r, thresholdImg)) {
	//		rectangle(display, r, Scalar(0, 0, 255), 1, 8, 0);
	//		R.push_back(r);
	//	}
	//}
	//imshow("Possible Char", display);
	//waitKey(0);


	vector<Mat> plate = dp.findPlate(srcImg);
	if (plate.size() == 0) {
		cout << "Cannot find plate";
		cin.get();
		return 0;
	}
	else {
		cout << "Found " << plate.size() << " Possible Plate" << endl;
	}
	dc.trainSVM("C:\\Users\\Huy\\Desktop\\Training2");
	dc.trainKNN("C:\\Users\\Huy\\Desktop\\Training2");
	for (int i = 0; i < plate.size(); i++) {
		imshow("Possible Plate " + to_string(i + 1), plate[i]);
		imwrite("result.png", plate[i]);
		//imshow("result.png", Preprocess::adaptiveThreshold(Preprocess::convertToGray(plate[i]), THRESH_BINARY, 19, 9));
		waitKey(0);
		system("tesseract result.png out > D:\\Document\\C++\\ANPR\\ANPR\\OutputDir\\log.txt 2>&1 ");
		cout << endl;
		ifstream f("out.txt");
		if (f.tellg() == 0) {
			cout << "No tesseract result for possible plate " << i + 1 << endl;
		}
		else {
			while (!f.eof()) {
				string temp = result;
				f >> result;
				cout << "Tesseract result for possible plate " << i + 1 << ": ";
				if (result.size() > 4 && result.size() < 8 && result != temp)
					cout << result << endl;
			}
		}
		string s1 = dc.detectSVMString(dc.splitChar(plate[i]));
		if (s1.length() > 0)
			cout << "SVM result for possible plate " << i + 1 << ": " << s1 <<  endl;
		else cout << "No SVM result for possible plate " << i + 1 << endl;
		s1 = dc.detectKNNString(dc.splitChar(plate[i]));
		if (s1.length() > 0)
			cout << "KNN result for possible plate " << i + 1 << ": " << s1 << endl;
		else cout << "No KNN result for possible plate " << i + 1 << endl << endl;
	}
		//for (int i = 0; i < plate.size(); i++) {
		//	cout << "SVM result: ";
		//	vector<Mat> charImg = dt.splitChar(plate[i]);
		//	for (int j = 0; j < charImg.size(); j++) {
		//		//imshow(to_string(j), charImg[j]);
		//		cout << dt.detectSVM(charImg[j]) << " ";
		//	}
		//	cout << endl << "KNN result: ";
		//	for (int j = 0; j < charImg.size(); j++) {
		//		//imshow(to_string(j), charImg[j]);
		//		cout << dt.detectKNN(charImg[j]) << " ";
		//	}
		//	cout << endl;
		//}
	//dc.trainSVM1("C:\\Users\\Huy\\Desktop\\Training");
	cout << endl << "Performance Of OCR"<< endl;
	char i = '0';
	int test = 0, total = 0;
	for (auto &p : fs::directory_iterator("C:\\Users\\Huy\\Desktop\\Training1")) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			if (temp.channels() != 1)
				temp = Preprocess::convertToGray(temp);
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32F);
			Mat detect(30, 30, CV_32F);
			dc.knn->findNearest(temp.reshape(1, 1), 1, detect);
			float r = (float)detect.at<float>(0, 0);
			total++;
			if (int(r) == i) test++;
			temp.~Mat();
		}
		if (i != int('9'))
			i++;
		else i = int('A');
	}
	cout << "Performance Of KNN :" << test * 100 / (float) total << endl;

	i = '0';
	test = 0;
	total = 0;
	for (auto &p : fs::directory_iterator("C:\\Users\\Huy\\Desktop\\Training1")) {
		for (auto &q : fs::directory_iterator(p)) {
			Mat temp = imread(q.path().string(), IMREAD_UNCHANGED);
			if (temp.channels() != 1)
				temp = Preprocess::convertToGray(temp);
			resize(temp, temp, Size(30, 30));
			temp.convertTo(temp, CV_32F);
			float r = dc.svm->predict(temp.reshape(1, 1));
			total++;
			if (int(r) == i) test++;
		}
		if (i != int('9'))
			i++;
		else i = int('A');
	}
	cout << "Performance Of SVM :" << test * 100 / (float)total << endl;
	cin.get();
	return 0;

}