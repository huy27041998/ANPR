#include "GenData.h"

bool GenData::sortRectX(const Rect &r1, const Rect &r2)
{
	return (r1.x < r2.x && abs(r1.y - r2.y) < 10);
}

bool GenData::sortRectY(const Rect &r1, const Rect &r2)
{
	return (r1.y < r2.y );
}

void GenData::GenData1(const char * path, int startNumber)
{
	const int MIN_CONTOUR_AREA = 100;

	const int RESIZED_IMAGE_WIDTH = 20;
	const int RESIZED_IMAGE_HEIGHT = 30;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	cv::Mat imgTrainingNumbers;         // input image
	cv::Mat imgGrayscale;               // 
	cv::Mat imgBlurred;                 // declare various images
	cv::Mat imgThresh;                  //
	cv::Mat imgThreshCopy;              //

	std::vector<std::vector<cv::Point> > ptContours;        // declare contours vector
	std::vector<cv::Vec4i> v4iHierarchy;                    // declare contours hierarchy

	cv::Mat matClassificationInts;      // these are our training classifications, note we will have to perform some conversions before writing to file later

										// these are our training images, due to the data types that the KNN object KNearest requires, we have to declare a single Mat,
										// then append to it as though it's a vector, also we will have to perform some conversions before writing to file later
	cv::Mat matTrainingImagesAsFlattenedFloats;

	// possible chars we are interested in are digits 0 through 9 and capital letters A through Z, put these in vector intValidChars
	std::vector<int> intValidChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z' };

	imgTrainingNumbers = cv::imread("training_chars2.png");          // read in training numbers image

	cv::cvtColor(imgTrainingNumbers, imgGrayscale, COLOR_BGR2GRAY);        // convert to grayscale

	cv::GaussianBlur(imgGrayscale,              // input image
		imgBlurred,                             // output image
		cv::Size(5, 5),                         // smoothing window width and height in pixels
		0);                                     // sigma value, determines how much the image will be blurred, zero makes function choose the sigma value

												// filter image from grayscale to black and white
	cv::adaptiveThreshold(imgBlurred,           // input image
		imgThresh,                              // output image
		255,                                    // make pixels that pass the threshold full white
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,         // use gaussian rather than mean, seems to give better results
		cv::THRESH_BINARY_INV,                  // invert so foreground will be white, background will be black
		11,                                     // size of a pixel neighborhood used to calculate threshold value
		2);                                     // constant subtracted from the mean or weighted mean

	imgThreshCopy = imgThresh.clone();          // make a copy of the thresh image, this in necessary b/c findContours modifies the image

	cv::findContours(imgThreshCopy,             // input image, make sure to use a copy since the function will modify this image in the course of finding contours
		ptContours,                             // output contours
		v4iHierarchy,                           // output hierarchy
		cv::RETR_EXTERNAL,                      // retrieve the outermost contours only
		cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points

	for (int i = 0; i < ptContours.size(); i++) {                           // for each contour
		if (cv::contourArea(ptContours[i]) > MIN_CONTOUR_AREA) {                // if contour is big enough to consider
			cv::Rect boundingRect = cv::boundingRect(ptContours[i]);                // get the bounding rect

			cv::rectangle(imgTrainingNumbers, boundingRect, cv::Scalar(0, 0, 255), 2);      // draw red rectangle around each contour as we ask user for input

			cv::Mat matROI = imgThresh(boundingRect);           // get ROI image of bounding rect

			cv::Mat matROIResized;
			cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

			cv::imshow("matROI", matROI);                               // show ROI image for reference
			cv::imshow("matROIResized", matROIResized);                 // show resized ROI image for reference
			cv::imshow("imgTrainingNumbers", imgTrainingNumbers);       // show training numbers image, this will now have red rectangles drawn on it

			int intChar = cv::waitKey(0);           // get key press

			if (intChar == 27) {        // if esc key was pressed
				return;              // exit program
			}
			else if (std::find(intValidChars.begin(), intValidChars.end(), intChar) != intValidChars.end()) {     // else if the char is in the list of chars we are looking for . . .

				matClassificationInts.push_back(intChar);       // append classification char to integer list of chars

				cv::Mat matImageFloat;                          // now add the training image (some conversion is necessary first) . . .
				matROIResized.convertTo(matImageFloat, CV_32FC1);       // convert Mat to float

				cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);       // flatten

				matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);       // add to Mat as though it was a vector, this is necessary due to the
																							// data types that KNearest.train accepts
			}   // end if
		}   // end if
	}
}

GenData::GenData(const char* path, int startNumber)
{
	Mat sourceImg = imread(path, IMREAD_UNCHANGED);
	Mat thresholdImg = Preprocess::adaptiveThreshold(Preprocess::convertToGray(sourceImg), 19, 9);
	//cout << w << " " << h << endl;
	//Mat thresholdImg = Preprocess::convertToGray(sourceImg);  (sourceImg, thresholdImg, 127, 255, THRESH_BINARY_INV);
	vector<Mat> character;
	vector<vector<Point>> contours;
	Mat temp1 = sourceImg.clone();
	//Tìm viền
	findContours(thresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//Vẽ viền
	/*for (int i = 0; i < contours.size(); i++) 
		drawContours(temp1, contours, i, Scalar(0,0,255), 1, 8, noArray(), INT_MAX, Point(0, 0));	*/
	vector <Rect> R; //Lưu tọa độ của bao viền
	for (int i = 0; i < contours.size(); i++) {
		Rect r = boundingRect(contours[i]);
		rectangle(temp1, r, Scalar(0, 0, 255), 1, 8, 0);
		R.push_back(r);
	}
	if (R.size() == 0) {
		cout << "Khong tim thay chu cai." << endl;
	}
	
	for (int i = 0; i < R.size(); i++) {
		for (int j = i + 1; j < R.size(); j++) {
			if (abs(R[i].y - R[j].y) > 2)
				swap(R[i], R[j]);
		}
	}
	for (int i = 0; i < R.size(); i++) {
		for (int j = i + 1; j < R.size(); j++) {
			if (R[i].x > R[j].x && abs(R[i].y - R[j].y) < 2)
				swap(R[i], R[j]);
		}
	}
	for (int i = 0; i < R.size(); i++) {
		Mat temp(sourceImg, R[i]);
		cout << to_string(i + startNumber) + ".png" << endl;
		imwrite(to_string(i + startNumber) + ".png", temp);
		temp.~Mat();
	}
	//cout << character.size() << endl;
	waitKey(0);
}


GenData::~GenData()
{
}
