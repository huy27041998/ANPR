#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
#include "DetectChar.h"
#include "GenData.h"
#include "DetectPlate.h"
using namespace std;
using namespace cv;


int main()
{
	//GenData g("C:\\Users\\Huy\\Desktop\\training2.png", 72);
	Mat sourceImg = imread("image1.png", IMREAD_UNCHANGED);
	if (sourceImg.empty()) {
		cout << "Cannot open image" << endl;
		cin.get();
		return 0;
	}
	resize(sourceImg, sourceImg, Size(sourceImg.cols / 2, sourceImg.rows / 2)); //làm nhỏ ảnh để tăng tốc độ xử lý
	Mat thresholdImg = Preprocess::thresholdImage(Preprocess::convertToGray(sourceImg), 9, 19);
	DetectPlate dp;
	vector<Mat> plate = dp.findPlate(sourceImg);
	//Mat p = plate[0];
	//imshow("plate", p);
	/*DetectChar dt;
	dt.train("C:\\Users\\Huy\\Desktop\\Training");
	Mat grayimg = Preprocess::convertToGray(p).clone();
	Mat thresholdimg = Preprocess::thresholdImage(grayimg, 19, 9).clone();
	vector<Mat> character = dt.splitChar(thresholdimg, p);
	for (int i = 0; i < character.size(); i++) {
		string s = to_string(i);
		imshow(s, character[i]);
		cout << dt.detect(character[i]);
	}*/
	waitKey(0);
	cin.get();
	return 0;
}