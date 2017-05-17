#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <ctime>
#include <memory>

#include "opencvcamera.h"
#include "depthmap.h"

class StereoCamera {
public:
	// constructor from 2 cameras
	StereoCamera(OpenCVCamera& left, OpenCVCamera& right);

	// destructor
	virtual ~StereoCamera();

	// get the next frame and compute the depth map
	std::chrono::time_point<std::chrono::system_clock> nextFrame();

	// get the disparity matricies
	cv::Mat* getDisparity();
	cv::Mat* getDisparityNorm();

	short getDispAt(int r, int c);
	unsigned char getDispAtNorm(int r, int c);

	// get the cameras
	OpenCVCamera* getLeftCamera();
	OpenCVCamera* getRightCamera();

	// get camera constants
	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	DepthMap _dpMap;

	cv::Mat _disparity;
	cv::Mat _disparityNorm;

	OpenCVCamera _cameraLeft;
	OpenCVCamera _cameraRight;
};

#endif