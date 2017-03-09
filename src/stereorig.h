#pragma once
#include "opencvcamera.h"

class StereoRig
{

	struct StereoCapture
	{
		cv::Mat* left;
		cv::Mat* right;

		StereoCapture(cv::Mat* left_image, cv::Mat* right_image) : left(left_image), right(left_image) {};

		StereoCapture::~StereoCapture()
		{
			delete left;
			delete right;
		};
	};

public:
	StereoRig(unsigned int id_left, unsigned int id_right);
	StereoRig(OpenCVCamera* camera1, OpenCVCamera* camera2);
	
	void Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);

	//user is responsible for cleaning up capture
	StereoCapture* Capture();



private:
	OpenCVCamera* _camera_left;
	OpenCVCamera* _camera_right;
};