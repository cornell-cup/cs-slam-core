#ifndef _SLAM_DISPARITY_PIPE
#define _SLAM_DISPARITY_PIPE

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <memory>

#define CAMERA_MATRIX_LEFT_PATH "cam_mats_left"
#define CAMERA_MATRIX_RIGHT_PATH "cam_mats_right"
#define DISTANCE_COEFF_LEFT_PATH "dist_coefs_left"
#define DISTANCE_COEFF_RIGHT_PATH "dist_coefs_right"

class DisparityPipeline {
private:
	cv::Mat _cameraMatrixLeft;
	cv::Mat _cameraMatrixRight;
	cv::Mat _distanceCoeffLeft;
	cv::Mat _distanceCoeffRight;

	cv::VideoCapture _cameraLeft;
	cv::VideoCapture _cameraRight;

	cv::Ptr<cv::StereoSGBM> _stereoSGBM;

	std::shared_ptr<cv::Mat> _disparity;

	cv::Mat readMatFromTxt(std::string filename, int rows, int cols);

public:
	DisparityPipeline(int width, int height, int rate, int cam_idx_left, int cam_idx_right, std::string calib_directory);

	void executePipeline();

	const std::shared_ptr<cv::Mat> getDisparity();
};

#endif