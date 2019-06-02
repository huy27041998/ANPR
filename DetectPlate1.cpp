#include "DetectPlate1.h"




vector<Mat> DetectPlate1::findPlate(Mat srcImg)
{
	vector <Mat> possiblePlate;
	vector<vector<Point>> contours;
	vector<RotatedRect> R;
	Mat temp = srcImg.clone();
	Mat grayImg = Preprocess::convertToGray(temp);
	Mat thresholdImg = Preprocess::simpleThreshold(grayImg, 127);
	/*imshow("threshold", thresholdImg);
	waitKey(0);*/
	findContours(thresholdImg, contours, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	//imshow("threholdImg", thresholdImg);
	//waitKey(0);
	Mat display = srcImg.clone();
	/*for (int i = 0; i < contours.size(); i++)
		drawContours(display, contours, i, Scalar(0, 0, 255), 1, 8);*/
	
	for (int i = 0; i < contours.size(); i++) {
		RotatedRect r = minAreaRect(contours[i]);
		if (verifySizes(r, grayImg)) {
			cv::Point2f vertices2f[4];
			r.points(vertices2f);
			Point vertices[4];
			for (int i = 0; i < 4; ++i) {
				vertices[i] = vertices2f[i];
			}
			draw(r, display);
			R.push_back(r);
		}
	}
	imshow("Possible Plate", display);
	waitKey(0);
	for (int i = 0; i < R.size(); i++) {
		RotatedRect minRect = R[i];
		float angle = minRect.angle;
		if (minRect.size.width < minRect.size.height)
			angle = 90 + angle;
		Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1.0);
		//for (int i = 0; i < rotmat.rows; i++) {
		//	for (int j = 0; j < rotmat.cols; j++)
		//		cout << rotmat.at<double>(i, j) << " ";
		//	cout << endl;
		//}
		//cout << endl;
		Mat img_rotated;
		warpAffine(temp, img_rotated, rotmat, srcImg.size());
		//cout << minRect.center.x << " " << minRect.center.y << " " << angle;
		/*imshow("rotated", img_rotated);
		waitKey(0);*/
		Size rect_size = minRect.size;
		if (minRect.size.width < minRect.size.height)
			swap(rect_size.width, rect_size.height);
		/*cv::Point2f vertices2f[4];
		minRect.points(vertices2f);
		Point vertices[4];
		for (int i = 0; i < 4; ++i) {
			vertices[i] = vertices2f[i];
		}
		cv::fillConvexPoly(img_rotated, vertices, 4, Scalar(0, 0, 255));*/
		int x = minRect.center.x;
		int y = minRect.center.y;
		int w = minRect.size.width;
		int h = minRect.size.height;
		if (w < h) swap(w, h);
		if (x - w / 2 > 0 && y - h / 2 > 0 &&  x + w < img_rotated.cols && y + h < img_rotated.rows && w > 0 && h > 0) {
			Rect r = Rect(x - w / 2, y - h / 2, w, h);
			Mat img_crop(img_rotated, r);
			//imshow("imgCrop", img_crop);
			//waitKey(0);
			possiblePlate.push_back(img_crop);
		}
	}
	return possiblePlate;
}

bool DetectPlate1::verifySizes(RotatedRect candidate, Mat grayImg)
{
	int width = candidate.size.width;
	int height = candidate.size.height;
	int area = width * height;
	if (width < height) swap(width, height);
	float ratio = 0;
	//if (width > 100 ) cout << width << " " << height << " ratio : " << ratio << endl;
	if (width > MIN_PLATE_WIDTH && width < MAX_PLATE_WIDTH && height > MIN_PLATE_HEIGTH && height < MAX_PLATE_HEIGTH){
		Mat plate;
		getRectSubPix(grayImg, Size2f(width, height), candidate.center, plate);
		Mat thresholdImg = Preprocess::simpleThreshold(plate, 127);
		ratio = (float) countNonZero(thresholdImg) / area;
		//cout << width << " " << height << " ratio : " << ratio << endl;
		return (ratio < MAX_PLATE_RATIO && ratio > MIN_PLATE_RATIO);
	}
	return false;
}

void DetectPlate1::draw(RotatedRect rect, Mat &srcImg)
{
	cv::Point2f vertices2f[4];
	rect.points(vertices2f);
	Point vertices[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i] = vertices2f[i];
	}
	for (int i = 0; i < 4; i++)
		line(srcImg, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 1, 8, 0);
}

DetectPlate1::DetectPlate1()
{
}


DetectPlate1::~DetectPlate1()
{
}
