#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
#include "DetectChar.h"
#include "GenData.h"
#include "DetectPlate.h"
#include "DetectPlate1.h"
using namespace std;
using namespace cv;


int main()
{
	Mat srcImg = imread("image1.png", IMREAD_UNCHANGED);
	if (!srcImg.empty()) {
		cout << "Cannot find image";
		return 0;
	}
	DetectPlate dp;
	DetectChar dt;
	vector<Mat> plate = dp.findPlate(srcImg);
	if (plate.size() == 0) {
		cout << "Cannot find plate";
		return 0;
	}
	for (int i = 0; i < plate.size(); i++) {
		imshow(to_string(i), plate[i]);
	}
	waitKey(0);
	for (int i = 0; i < plate.size(); i++) {
		vector<Mat> charImg = dt.splitChar(srcImg);
		for (int j = 0; j < charImg.size(); j++) {
			imshow(to_string(j), charImg[j]);
		}
		
	}


	waitKey(0);
	cin.get();
	return 0;
}