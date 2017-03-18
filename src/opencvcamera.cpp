#include "opencvcamera.h"

void OpenCVCamera::Configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
{
		_capture.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
		_capture.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
		_capture.set(CV_CAP_PROP_FPS, frame_rate);
	}
unsigned int OpenCVCamera::getHeight() const
{
	return getAttribute(CV_CAP_PROP_FRAME_HEIGHT);
}

unsigned int OpenCVCamera::getWidth() const
{
	return getAttribute(CV_CAP_PROP_FRAME_WIDTH);
}

unsigned int OpenCVCamera::getFrameRate() const
{
	return getAttribute(CV_CAP_PROP_FPS);
}

unsigned int OpenCVCamera::getAttribute(unsigned int id) const
{
	return _capture.get(id);
}
