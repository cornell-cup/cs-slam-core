#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

#include <opencv2/core/core.hpp>

// camera class interface
class Camera {
public:
	virtual void capture(cv::Mat& dest) = 0;

	virtual unsigned int getHeight() const = 0;
	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getFrameRate() const = 0;
};

#endif
