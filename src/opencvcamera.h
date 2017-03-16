#ifndef _SLAM_OPENCVCAMERA
#define _SLAM_OPENCVCAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class OpenCVCamera
{
public:
	OpenCVCamera(unsigned int id) : _id(id) , _capture(_id){};

	void Capture(cv::Mat& dest)
	{
		_capture >> dest;
	};

	void Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
	{
		_capture.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
		_capture.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
		_capture.set(CV_CAP_PROP_FPS, frame_rate);
	}

	unsigned int getHeight() const
	{
		return _capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	}

	unsigned int getWidth() const
	{
		return _capture.get(CV_CAP_PROP_FRAME_WIDTH);
	}

	unsigned int getFrameRate() const
	{
		return _capture.get(CV_CAP_PROP_FPS);
	}

	unsigned int getAttribute(unsigned int id) const
	{
		return _capture.get(id);
	}
	
protected:
	int _id;


private:
	cv::VideoCapture _capture;
};

#endif