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

	void Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);

	unsigned int getHeight() const;

	unsigned int getWidth() const;

	unsigned int getFrameRate() const;

	unsigned int getAttribute(unsigned int id) const;
	
protected:
	int _id;


private:
	cv::VideoCapture _capture;
};

#endif