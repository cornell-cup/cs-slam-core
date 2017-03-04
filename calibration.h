#ifndef _CALIBRATION
#define _CALIBRATION

#include <opencv2/core/core.hpp>

#define CAMERA_MATRIX_LEFT_PATH "PATHLEFT"
#define CAMERA_MATRIX_RIGHT_PATH "PATHRIGHT"
#define DISTANCE_COEFF_LEFT_PATH "DPATHLEFT"
#define DISTANCE_COEFF_RIGHT_PATH "DPATHRIGHT" // TODO fill in exact values

class Calibration {
private:
	cv::Mat _cameraMatrixLeft;
	cv::Mat _cameraMatrixRight;
	cv::Mat _distanceCoeffLeft;
	cv::Mat _distanceCoeffRight;

public:
	Calibration(); //load in the matrices
	virtual ~Calibration();

	void undistortImages(cv::Mat& leftImg, cv::Mat& rightImg, cv::Mat& leftImageDest, cv::Mat& rightImageDest);

};

#endif