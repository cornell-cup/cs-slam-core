#include "stereocamera.h"

StereoCamera::StereoCamera(OpenCVCamera& left, OpenCVCamera& right): _cameraLeft(left), _cameraRight(right) {}

StereoCamera::~StereoCamera() {}

std::shared_ptr<StereoCamera::StereoCapture> StereoCamera::getImage(){

	StereoCapture* cap = new StereoCapture();

	_cameraLeft.capture(cap->left);
	_cameraRight.capture(cap->right);
	cap->stamp = std::chrono::system_clock::now();

	return std::shared_ptr<StereoCapture>(cap);
}

unsigned int StereoCamera::getWidth() const
{
	return _cameraLeft.getWidth();
}

unsigned int StereoCamera::getHeight() const
{
	return _cameraLeft.getHeight();
}
