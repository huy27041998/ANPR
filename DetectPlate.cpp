#include "DetectPlate.h"



vector<Mat> DetectPlate::findPlate(Mat srcImg)
{
	vector <Mat> plate;
	vector<vector<Point>> contours;
	vector<Rect> R;
	Mat temp = srcImg.clone();
	srcImg = Preprocess::convertToGray(srcImg);
	srcImg = Preprocess::gBlur(srcImg);
	srcImg = Preprocess::thresholdImage(srcImg, 19, 9);
	findContours(srcImg, contours, RETR_LIST ,CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*for (int i = 0; i < contours.size(); i++) {
		drawContours(temp, contours, i, Scalar(0, 0, 255), 1, 8, noArray(), INT_MAX, Point(0, 0));
	}*/
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		float w = r.width, h = r.height, s = w * h;
		float ratio = ((w == 0) ? 0 : (h == 0) ? 0 : h / w);
		Mat temp1(srcImg, r);
		float ratiobw = (float)countNonZero(temp1) / s;
		float s = r.area();
		if (ratio > 0.9) {
			cout << w << " " << h << " " << ratio << " " << ratiobw << endl;
			rectangle(temp, r, Scalar(255, 0, 255), 1, 8, 0);
			Mat temp2(temp, r);
			plate.push_back(temp2);
		}
		
		temp1.~Mat();
	}
	imshow("plate", temp);
	waitKey(0);
	return plate;
}

DetectPlate::DetectPlate()
{
}


DetectPlate::~DetectPlate()
{
}
