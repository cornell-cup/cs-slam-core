#include "calibration.h" 

Calibration::Calibration() {
	cv::Mat test = cv::Mat::eye(3, 3, CV_32F);
	_cameraMatrixLeft = readMatFromTxt(MAT_ROOT_PATH + std::string("/")+ CAMERA_MATRIX_LEFT_PATH, 3, 3);
	_cameraMatrixRight = readMatFromTxt(MAT_ROOT_PATH + std::string("/") + CAMERA_MATRIX_RIGHT_PATH, 3, 3);
	_distanceCoeffLeft = readMatFromTxt(MAT_ROOT_PATH + std::string("/") + DISTANCE_COEFF_LEFT_PATH, 1, 5);
	_distanceCoeffRight = readMatFromTxt(MAT_ROOT_PATH + std::string("/") + DISTANCE_COEFF_RIGHT_PATH, 1, 5);
}

Calibration::~Calibration() {}

void Calibration::undistortImages(cv::Mat& imgLeft, cv::Mat& imgRight, cv::Mat& imgLeftDest, cv::Mat& imgRightDest) {
	cv::undistort(imgLeft, imgLeftDest, _cameraMatrixLeft, _distanceCoeffLeft);
	cv::undistort(imgRight, imgRightDest, _cameraMatrixRight, _distanceCoeffRight);
}

cv::Mat Calibration::readMatFromTxt(std::string filename, int rows, int cols){
	int m;
	cv::Mat out = cv::Mat::zeros(rows, cols, CV_32S); //Matrix to store values

	std::ifstream fileStream(filename);
	int cnt = 0;//index starts from 0
	while (fileStream >> m)
	{
		int temprow = cnt / cols;
		int tempcol = cnt % cols;
		out.at<int>(temprow, tempcol) = m;
		cnt++;
	}
	return out;
}