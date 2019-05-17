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
	Mat srcImg = imread("image4.png", IMREAD_UNCHANGED);
	//GenData gen("C:\\Users\\Huy\\Desktop\\car_font.png", 180);
	if (srcImg.empty()) {
		cout << "Cannot find image";
		cin.get();
		return 0;
	}
	imshow("Source Image", srcImg);
	waitKey(0);
	DetectPlate dp;
	DetectChar dt;
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
		return 0;
	}
	dt.trainSVM("C:\\Users\\Huy\\Desktop\\Training");
	dt.trainKNN("C:\\Users\\Huy\\Desktop\\Training");
	//dt.trainKNNUsingXML();
	for (int i = 0; i < plate.size(); i++) {
		imshow("Possible Plate " + to_string(i + 1), plate[i]);
		//imwrite("test.jpg", plate[i]);
		for (int i = 0; i < plate.size(); i++) {
			cout << "SVM result: ";
			vector<Mat> charImg = dt.splitChar(plate[i]);
			for (int j = 0; j < charImg.size(); j++) {
				//imshow(to_string(j), charImg[j]);
				cout << dt.detectSVM(charImg[j]) << " ";
			}
			cout << endl << "KNN result: ";
			for (int j = 0; j < charImg.size(); j++) {
				//imshow(to_string(j), charImg[j]);
				cout << dt.detectKNN(charImg[j]) << " ";
			}
			cout << endl;
		}
	}
	cin.get();
	return 0;
}