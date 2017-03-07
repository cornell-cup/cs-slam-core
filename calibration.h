#ifndef _CALIBRATION
#define _CALIBRATION

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <fstream>

#define CAMERA_MATRIX_LEFT_PATH "cam_mats_left"
#define CAMERA_MATRIX_RIGHT_PATH "cam_mats_right"
#define DISTANCE_COEFF_LEFT_PATH "dist_coefs_left"
#define DISTANCE_COEFF_RIGHT_PATH "dist_coefs_right" // TODO fill in exact values

#define MAT_ROOT_PATH "calubration_mats"

class Calibration {
private:
	cv::Mat _cameraMatrixLeft;
	cv::Mat _cameraMatrixRight;
	cv::Mat _distanceCoeffLeft;
	cv::Mat _distanceCoeffRight;

	cv::Mat readMatFromTxt(std::string filename, int rows, int cols);

public:
	Calibration(); //load in the matrices
	virtual ~Calibration();

	void undistortImages(cv::Mat& leftImg, cv::Mat& rightImg, cv::Mat& leftImageDest, cv::Mat& rightImageDest);

};

#endif