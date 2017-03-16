#include "stereocamera.h"

StereoCamera::StereoCamera(unsigned int width, unsigned int height, unsigned int rate, int cam_idx_left, int cam_idx_right) :
							_cameraLeft(cam_idx_left), _cameraRight(cam_idx_right){
	configure(width, height, rate);
}

StereoCamera::StereoCamera(unsigned int width, unsigned int height, unsigned frame_rate, OpenCVCamera & left, OpenCVCamera & right) :
							_cameraLeft(left), _cameraRight(right)
{

}

StereoCamera::~StereoCamera() {}

void StereoCamera::configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
{
	_cameraLeft.Configure(frame_width, frame_height, frame_rate);
	_cameraRight.Configure(frame_width, frame_height, frame_width);
}

std::shared_ptr<StereoCamera::StereoCapture> StereoCamera::getImage(){

	StereoCapture* cap = new StereoCapture();

	_cameraLeft.Capture(cap->left);
	_cameraRight.Capture(cap->right);
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
