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
	struct StereoCapture
	{
		cv::Mat left;
		cv::Mat right;
		std::chrono::time_point<std::chrono::system_clock> stamp;

		StereoCapture(cv::Mat& left_image, cv::Mat& right_image, std::chrono::time_point<std::chrono::system_clock> time_stamp) : left(left_image), right(left_image), stamp(time_stamp) {};
		StereoCapture() {};

		bool isValid()
		{
			return !left.empty() && !right.empty();
		}
	};

	StereoCamera(OpenCVCamera& left, OpenCVCamera& right);

	virtual ~StereoCamera();

	std::shared_ptr<StereoCamera::StereoCapture> getImage();

	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	OpenCVCamera _cameraLeft;
	OpenCVCamera _cameraRight;

};

#endif