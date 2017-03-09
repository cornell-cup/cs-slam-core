#pragma once
#ifndef _UTIL_
#define _UTIL_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Util {
public:
	static void toGrayscale(cv::Mat& imgSrc, cv::Mat& imgDest);

	static void normalize(cv::Mat& input, cv::Mat& output);
};

#endif _UTIL_