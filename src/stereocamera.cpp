#include "stereocamera.h"

StereoCamera::StereoCamera(OpenCVCamera& left, OpenCVCamera& right): _cameraLeft(left), _cameraRight(right) {
	// intialize the depth map pipeline step
	_dpMap = DepthMap();

	// initialize disparity and disparity norm
	_disparity = cv::Mat::zeros(_cameraLeft.getHeight(), _cameraLeft.getWidth(), CV_8U);
	_disparityNorm = cv::Mat::zeros(_cameraLeft.getHeight(), _cameraLeft.getWidth(), CV_8U);
}

StereoCamera::~StereoCamera() {}

// capture images from both camera's an put the results in the corresponding matricies
std::chrono::time_point<std::chrono::system_clock> StereoCamera::nextFrame(){

	_cameraLeft.nextFrame();
	_cameraRight.nextFrame();

	cv::Mat leftG, rightG;

	// TODO see if this only needs to be done once, not a major issue though
	cv::cvtColor(*(_cameraLeft.getFrame()), leftG, cv::COLOR_BGR2GRAY);
	cv::cvtColor(*(_cameraRight.getFrame()), rightG, cv::COLOR_BGR2GRAY);
	
	// calculate the disprity map
	_dpMap.getDisparity(leftG, rightG, _disparity);
	
	// normalize the disparity map for display
	cv::normalize(_disparity, _disparityNorm, 0, 255, cv::NORM_MINMAX, CV_8U);

	return std::chrono::system_clock::now();
}

// get the disparity matricies
cv::Mat* StereoCamera::getDisparity() {
	return &_disparity;
}

cv::Mat* StereoCamera::getDisparityNorm() {
	return &_disparityNorm;
}

short StereoCamera::getDispAt(int r, int c) {
	return _disparity.at<short>(r,c);
}

unsigned char StereoCamera::getDispAtNorm(int r, int c) {
	return _disparityNorm.at<unsigned char>(r, c);
}

// get the cameras
OpenCVCamera* StereoCamera::getLeftCamera() {
	return &_cameraLeft;
}

OpenCVCamera* StereoCamera::getRightCamera() {
	return &_cameraRight;
}

unsigned int StereoCamera::getWidth() const{
	return _cameraLeft.getWidth();
}

unsigned int StereoCamera::getHeight() const
{
	return _cameraLeft.getHeight();
}
