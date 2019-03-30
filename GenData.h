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
	void GenData1(const char * path, int startNumber);
	GenData(const char * path, int startNumber);
	~GenData();
};

