#include "DetectPlate1.h"




vector<Mat> DetectPlate1::findPlate(Mat srcImg)
{
	vector <Mat> possiblePlate;
	vector<vector<Point>> contours;
	vector<RotatedRect> R;
	Mat temp = srcImg.clone();
	temp = Preprocess::convertToGray(temp);
	//srcImg = Preprocess::gBlur(srcImg);
	temp = Preprocess::simpleThreshold(temp, 127);
	findContours(temp, contours, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));
	//for (int i = 0; i < contours.size(); i++)
	//	drawContours(srcImg, contours, i, Scalar(0, 0, 255), 1, 8);
	Mat temp1 = srcImg.clone();
	for (int i = 0; i < contours.size(); i++) {
		RotatedRect r = minAreaRect(contours[i]);
		if (verifySizes(r)) {
			cv::Point2f vertices2f[4];
			r.points(vertices2f);
			Point vertices[4];
			for (int i = 0; i < 4; ++i) {
				vertices[i] = vertices2f[i];
			}
			cv::fillConvexPoly(temp1, vertices, 4, Scalar(0, 0, 255));
			R.push_back(r);
		}
	}
	imshow("temp1", temp1);
	for (int i = 0; i < R.size(); i++) {
		RotatedRect minRect = R[i];
		float r = (float)minRect.size.width / (float)minRect.size.height;
		float angle = minRect.angle;
		cout << angle;
		cout << r << endl;
		if (r < 1)
			angle = -(90 + angle);
		Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1.0);
		Mat img_rotated;
		warpAffine(srcImg, img_rotated, rotmat, srcImg.size());
		Size rect_size = minRect.size;
		if (r < 1)
			swap(rect_size.width, rect_size.height);
		cv::Point2f vertices2f[4];
		minRect.points(vertices2f);
		Point vertices[4];
		for (int i = 0; i < 4; ++i) {
			vertices[i] = vertices2f[i];
		}
		cv::fillConvexPoly(img_rotated, vertices, 4, Scalar(0, 0, 255));
		imshow("rotated", img_rotated);
		cv::waitKey(0);
		Mat img_crop;
		getRectSubPix(img_rotated, rect_size, minRect.center, img_crop, INTER_CUBIC);
		Mat resultResized;
		resultResized.create(33, 144, CV_8UC3);
		resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
		Mat grayResult;
		cvtColor(resultResized, grayResult, COLOR_BGR2GRAY);
		blur(grayResult, grayResult, Size(3, 3));
		equalizeHist(grayResult, grayResult);
		possiblePlate.push_back(Mat(grayResult, minRect.boundingRect()));
	}
	return vector<Mat>();
}

bool DetectPlate1::verifySizes(RotatedRect candidate)
{
	float error = 0.4;
	const float aspect = 2.08108;
	int min = 30 * aspect * 30; 
	int max = 90 * aspect * 90; 
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;
	int area = candidate.size.height * candidate.size.width;
	float r = (float)candidate.size.width / (float)candidate.size.height;
	if (r < 1)
	r = 1 / r;
	if ((area < min || area > max) || (r < rmin || r > rmax)) {
		return false;
	}
	else {
		return true;
	}
}

DetectPlate1::DetectPlate1()
{
}


DetectPlate1::~DetectPlate1()
{
}
