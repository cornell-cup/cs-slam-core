#pragma once
#ifndef _UTIL_
#define _UTIL_

#include <opencv2/core/core.hpp>

class Util {
public:
	static void toGrayscale(cv::Mat& imgSrc, cv::Mat& imgDest);
};

#endif _UTIL_