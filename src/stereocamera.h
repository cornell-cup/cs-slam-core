#ifndef _SLAM_STEREOCAMERA
#define _SLAM_STEREOCAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <ctime>
#include <memory>

#include "camera.h"

class StereoCamera {
public:

	StereoCamera(Camera* left, Camera* right);

	virtual ~StereoCamera();

	std::chrono::time_point<std::chrono::system_clock> getImage(cv::Mat& left, cv::Mat& right);

	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	Camera* _cameraLeft;
	Camera* _cameraRight;

};

#endif