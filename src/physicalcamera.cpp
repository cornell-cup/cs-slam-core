#include "physicalcamera.h"

PhysicalCamera::PhysicalCamera(unsigned int id) : _id(id) , _capture(_id){};

PhysicalCamera::~PhysicalCamera(){}

void PhysicalCamera::loadCalibration(std::string cam_mat, std::string dist_mat) {
	_cameraMatrix = _readMatFromTxt(cam_mat, 3, 3);
	_distanceCoeff = _readMatFromTxt(dist_mat, 1, 5);
}

void PhysicalCamera::configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
{
	_capture.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
	_capture.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
	_capture.set(CV_CAP_PROP_FPS, frame_rate);
}

void PhysicalCamera::capture(cv::Mat& dest){
	if(!(_cameraMatrix.empty() || _distanceCoeff.empty())) {
		cv::Mat tempdest;
		_capture >> tempdest;
		cv::undistort(tempdest, dest, _cameraMatrix, _distanceCoeff);
	}
	else {
		_capture >> dest;
	}
}

unsigned int PhysicalCamera::getHeight() const
{
	return getAttribute(CV_CAP_PROP_FRAME_HEIGHT);
}

unsigned int PhysicalCamera::getWidth() const
{
	return getAttribute(CV_CAP_PROP_FRAME_WIDTH);
}

unsigned int PhysicalCamera::getFrameRate() const
{
	return getAttribute(CV_CAP_PROP_FPS);
}

unsigned int PhysicalCamera::getAttribute(unsigned int id) const
{
	return _capture.get(id);
}


cv::Mat PhysicalCamera::_readMatFromTxt(std::string filename, int rows, int cols) {
	float m;
	cv::Mat out = cv::Mat::zeros(rows, cols, CV_32F); //Matrix to store values

	std::ifstream fileStream(filename);
	int cnt = 0;	//index starts from 0
	// loop over through the file stream and read the requested number of numbers
	while (fileStream >> m)
	{
		int temprow = cnt / cols;
		int tempcol = cnt % cols;
		out.at<float>(temprow, tempcol) = m;
		cnt++;
	}
	return out;
}