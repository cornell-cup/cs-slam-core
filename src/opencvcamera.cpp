#include "opencvcamera.h"

OpenCVCamera::OpenCVCamera(unsigned int id, unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate) : _capture(id){
	_configure(frame_width, frame_height, frame_rate);
};

OpenCVCamera::OpenCVCamera(std::string filename, unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate) : _capture(filename) {
	_configure(frame_width, frame_height, frame_rate);
};

OpenCVCamera::~OpenCVCamera(){}

// capture an image and undistort it
void OpenCVCamera::nextFrame(){
	if(!(_cameraMatrix.empty() || _distanceCoeff.empty())) {
		cv::Mat tempdest;
		// capture the image
		_capture >> tempdest;

		// potentially restart the video stream and retry
		if (tempdest.empty()) {
			_capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
			_capture >> tempdest;
		}

		// return if nothing
		if(tempdest.empty())
			return;

		// potentially set the undistort maps
		_setMaps(tempdest);

		// setup source and destination matricies for the image
		cv::Mat src = ((cv::InputArray)tempdest).getMat();
		_frame.create(src.size(), src.type());
		cv::Mat dst = ((cv::InputArray)_frame).getMat();

		// undistort calculations
		int stripe_size0 = std::min(std::max(1, (1 << 12) / std::max(src.cols, 1)), src.rows);
		int i = 0;
		for (int y = 0; y < tempdest.rows; y += stripe_size0)
		{
			int stripe_size = std::min(stripe_size0, src.rows - y);
			cv::Mat dst_part = dst.rowRange(y, y + stripe_size);
			
			remap(src, dst_part, _map1.at(i), _map2.at(i), cv::INTER_LINEAR, cv::BORDER_CONSTANT);
			i++;
		}
	}
	else {
		_capture >> _frame;
	}

	// nudge the frame up or down
	if (_nudgeAmount != 0)
		_translateImg(_frame, 0, _nudgeAmount);
}

cv::Mat* OpenCVCamera::getFrame() {
	return &_frame;
}

void OpenCVCamera::nudge(int amount) {
	_nudgeAmount += amount;
}

// load the calibration matricies from the specified files files
void OpenCVCamera::loadCalibration(std::string cam_mat, std::string dist_mat) {
	_cameraMatrix = _readMatFromTxt(cam_mat, 3, 3);
	_distanceCoeff = _readMatFromTxt(dist_mat, 1, 5);
}

// add configuration setup for width, height, and frame rate
void OpenCVCamera::_configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate)
{
	_capture.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
	_capture.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
	_capture.set(CV_CAP_PROP_FPS, frame_rate);
}

// initliaze the caibration matricies, copied from OpenCV code
void OpenCVCamera::_setMaps(cv::Mat& _src) {
	if (_map1.empty() || _map2.empty()) {
		// std::cout << "recalculate calibration maps" << std::endl;

		cv::Mat src = ((cv::InputArray )_src).getMat(), cameraMatrix = ((cv::InputArray)_cameraMatrix).getMat();
		cv::Mat distCoeffs = ((cv::InputArray)_distanceCoeff).getMat();

		cv::Mat newCameraMatrix;

		int stripe_size0 = std::min(std::max(1, (1 << 12) / std::max(src.cols, 1)), src.rows);

		cv::Mat_<double> A, Ar, I = cv::Mat_<double>::eye(3, 3);

		cameraMatrix.convertTo(A, CV_64F);
		if (!distCoeffs.empty())
			distCoeffs = cv::Mat_<double>(distCoeffs);
		else
		{
			distCoeffs.create(5, 1, CV_64F);
			distCoeffs = 0.;
		}

		if (!newCameraMatrix.empty())
			newCameraMatrix.convertTo(Ar, CV_64F);
		else
			A.copyTo(Ar);

		double v0 = Ar(1, 2);
		for (int y = 0; y < src.rows; y += stripe_size0)
		{
			int stripe_size = std::min(stripe_size0, src.rows - y);
			Ar(1, 2) = v0 - y;
			cv::Mat map1_part(stripe_size0, src.cols, CV_16SC2);
			cv::Mat map2_part(stripe_size0, src.cols, CV_16UC1);

			initUndistortRectifyMap(A, distCoeffs, I, Ar, cv::Size(src.cols, stripe_size),
				map1_part.type(), map1_part, map2_part);
			_map1.push_back(map1_part);
			_map2.push_back(map2_part);
		}
	}
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

cv::Mat OpenCVCamera::_translateImg(cv::Mat &img, int offsetx, int offsety) {
	cv::Mat trans_mat = (cv::Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	cv::warpAffine(img, img, trans_mat, img.size());
	return trans_mat;
}

cv::Mat OpenCVCamera::_readMatFromTxt(std::string filename, int rows, int cols) {
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