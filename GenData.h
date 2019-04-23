#pragma once
#include <iostream>
#include <filesystem>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Preprocess.h"
using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;
class GenData
{
public:
	static bool sortRectX(const Rect &r1, const Rect &r2); //Sắp xếp lại đường viền theo phương X
	static bool sortRectY(const Rect &r1, const Rect &r2); //Sắp xếp lại đường viền theo phương Y
	void GenData1(const char * path, int startNumber);
	GenData(const char * path, int startNumber);
	~GenData();
};

