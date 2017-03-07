#include "util.h"

void Util::toGrayscale(cv::Mat& imgSrc, cv::Mat& imgDest) {
	cv::cvtColor(imgSrc, imgDest, cv::COLOR_BGR2GRAY);
}

void PostProcess::normalize(cv::Mat& input, cv::Mat& output) {
	cv::normalize(input, output, 0, 255, cv::NORM_MINMAX, CV_8U);
}