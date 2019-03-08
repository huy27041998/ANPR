#include "DetectChar.h"



vector<Mat> DetectChar::splitChar(Mat thresholdImg, Mat sourceImg)
{
	int w = sourceImg.cols, h = sourceImg.rows;
	cout << w << " " << h << endl;
	Mat temp(sourceImg);

	vector<Mat> character;
	vector<vector<Point>> contours;
	//Tìm viền
	findContours(thresholdImg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); 
	//Vẽ viền
	for (int i = 0; i < contours.size(); i++) 
		drawContours(temp, contours, i, Scalar(255, 0, 0), 1, 8, noArray(), INT_MAX, Point(0, 0));	
	vector <Rect> R; //Lưu tọa độ của bao viền
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		if (0.1 * w < r.width && r.width < 0.2 * w  && 0.4 * h < r.height && r.height < 0.6 * h) {
			cout << r.width << " " << r.height << endl;
			rectangle(temp, r, Scalar(0, 0, 255), 1, 8, 0);
			R.push_back(r);
		}
	}
	imshow("Character", temp);
	waitKey(0);
	//temp.~Mat();
	////Sắp xếp 
	////Xếp theo hàng dọc
	//for (int i = 0; i < R.size() - 1; i++)
	//	for (int j = i + 1; j < R.size(); j++) {
	//		if (R[i].y > R[j].y)
	//			swap(R[i], R[j]);
	//	}
	////Xếp theo hàng ngang
	//for (int i = 0; i < R.size() - 1; i++)
	//	for (int j = i + 1; j < R.size(); j++) {
	//		if (R[i].x > R[j].x && fabs(R[i].y - R[j].y) < 0.2)
	//			swap(R[i], R[j]);
	//	}
	//for (int i = 0; i < contours.size(); i++) {
	//	Mat temp(sourceImg, R[i]);
	//	character.push_back(temp);
	//}
	return character;
}

DetectChar::DetectChar()
{
}


DetectChar::~DetectChar()
{
}
