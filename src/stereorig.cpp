#include "stereorig.h"

StereoRig::StereoRig(unsigned int id_left, unsigned int id_right)
{
	_camera_left = new OpenCVCamera(id_left);
	_camera_right = new OpenCVCamera(id_right);

	assert(_camera_left);
	assert(_camera_right);
}

StereoRig::StereoRig(OpenCVCamera * left, OpenCVCamera * right): _camera_left(left), _camera_right(right)
{
	assert(_camera_left);
	assert(_camera_right);
}

void StereoRig::Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
{
	_camera_left->Configure(frame_width,frame_height,frame_rate);
	_camera_right->Configure(frame_width,frame_height,frame_width);
}

StereoRig::StereoCapture * StereoRig::Capture()
{
	cv::Mat* left = _camera_left->Capture();
	cv::Mat* right = _camera_right->Capture();

	return new StereoCapture(left, right);
}
