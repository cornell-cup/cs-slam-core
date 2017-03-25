#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <ctime>
#include <memory>

#include "opencvcamera.h"

class StereoCamera {
public:

	StereoCamera(OpenCVCamera& left, OpenCVCamera& right);

	virtual ~StereoCamera();

	std::chrono::time_point<std::chrono::system_clock> getImage(cv::Mat& left, cv::Mat& right);

	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	OpenCVCamera _cameraLeft;
	OpenCVCamera _cameraRight;

};

#endif