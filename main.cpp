#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Preprocess.h"
#include "DetectChar.h"
using namespace std;
using namespace cv;


int main()
{
	Mat sourceImg = imread("image17.png", IMREAD_UNCHANGED);
	if (sourceImg.empty()) {
		cout << "Cannot open image" << endl;
		cin.get();
		return 0;
	}
	resize(sourceImg, sourceImg, Size(sourceImg.cols / 2, sourceImg.rows / 2)); //Làm nhỏ ảnh để tăng tốc độ xử lý
	Mat grayImg = Preprocess::convertToGray(sourceImg);
	Mat thresholdImg = Preprocess::thresholdImage(grayImg);
	imshow("Source Image", thresholdImg);
	DetectChar dc;
	dc.splitChar(thresholdImg, sourceImg);
	waitKey(0);
	//imwrite("image1.jpg", sourceImg);
	cin.get();
	return 0;
}