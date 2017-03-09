#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class OpenCVCamera
{
public:
	OpenCVCamera(unsigned int id) : _id(id) , _capture(_id){};

	//caller responsible for cleaning up each capture
	cv::Mat* Capture()
	{
		cv::Mat* mat = new cv::Mat();
		_capture >> *mat;

		return mat;
	};

	void Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
	{
		_capture.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
		_capture.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
		_capture.set(CV_CAP_PROP_FPS, frame_rate);
	}
	
protected:
	int _id;


private:
	cv::VideoCapture _capture;
};