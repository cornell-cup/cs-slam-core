#include "stereocamera.h"

StereoCamera::StereoCamera(Camera* left, Camera* right): _cameraLeft(left), _cameraRight(right) {}

StereoCamera::~StereoCamera() {}

std::chrono::time_point<std::chrono::system_clock> StereoCamera::getImage(cv::Mat& left, cv::Mat& right){

	_cameraLeft->capture(left);
	_cameraRight->capture(right);

	return std::chrono::system_clock::now();
}

unsigned int StereoCamera::getWidth() const
{
	return _cameraLeft->getWidth();
}

unsigned int StereoCamera::getHeight() const
{
	return _cameraLeft->getHeight();
}
