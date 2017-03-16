#include "disparity-pipe.h"

DisparityPipeline::DisparityPipeline(CameraConfig& cam_config, int cam_idx_left, int cam_idx_right, std::string calib_directory) {
  // camera setup
  _cameraLeft = cv::VideoCapture(cam_idx_left);
  _cameraLeft.set(CV_CAP_PROP_FRAME_WIDTH, cam_config.width);
  _cameraLeft.set(CV_CAP_PROP_FRAME_HEIGHT, cam_config.height);
  _cameraLeft.set(CV_CAP_PROP_FPS, cam_config.rate);

  _cameraRight = cv::VideoCapture(cam_idx_right);
  _cameraRight.set(CV_CAP_PROP_FRAME_WIDTH, cam_config.width);
  _cameraRight.set(CV_CAP_PROP_FRAME_HEIGHT, cam_config.height);
  _cameraRight.set(CV_CAP_PROP_FPS, cam_config.rate);

  // calibration setup
  _cameraMatrixLeft = readMatFromTxt(calib_directory + std::string("/")+ CAMERA_MATRIX_LEFT_PATH, 3, 3);
	_cameraMatrixRight = readMatFromTxt(calib_directory + std::string("/") + CAMERA_MATRIX_RIGHT_PATH, 3, 3);
	_distanceCoeffLeft = readMatFromTxt(calib_directory + std::string("/") + DISTANCE_COEFF_LEFT_PATH, 1, 5);
	_distanceCoeffRight = readMatFromTxt(calib_directory + std::string("/") + DISTANCE_COEFF_RIGHT_PATH, 1, 5);

  // TODO make struct and overload constructor
  // disparity setup
  int SADWindowSize = 20;
	int p1 = 8 * 1 * SADWindowSize * SADWindowSize;
	int p2 = 4 * p1;

	int minDisparity = 25;
	int numDisparities = 64;
	int blockSize = 9;
	int disp12MaxDiff = 1;
	int preFilterCap = 63;
	int uniquenessRatio = 7;
	int speckleWindowSize = 50;
	int speckleRange = 1;

	_stereoSGBM = cv::StereoSGBM::create(
		minDisparity,
		numDisparities,
		blockSize,
		p1,
		p2,
		disp12MaxDiff,
		preFilterCap,
		uniquenessRatio,
		speckleWindowSize,
		speckleRange
	);

  // setup display
  cv::namedWindow("left");
  cv::namedWindow("right");
  cv::namedWindow("disparity");
}

void DisexecutePipeline::executePipeline() {
	// TODO investigate removal of grayscale
	cv::Mat left, right, leftG, rightG;
	cv::Mat disparity = cv::Mat::zeros(HEIGHT, WIDTH, CV_8U);

	// get the images from the cameras
	_cameraLeft >> left;
  _cameraRight >> right;

	// convert the images to grayscale
	cv::cvtColor(left, leftG, cv::COLOR_BGR2GRAY);
	cv::cvtColor(right, rightG, cv::COLOR_BGR2GRAY);

	// undistort
	cv::undistort(leftG, left, _cameraMatrixLeft, _distanceCoeffLeft);
	cv::undistort(rightG, right, _cameraMatrixRight, _distanceCoeffRight);

	// compute disparity
	_stereoSGBM->compute(left, right, disparity);

	// normalize
	cv::normalize(disparity, disparity, 0, 255, cv::NORM_MINMAX, CV_8U);

	cv::imshow("left", left);
	cv::imshow("right", right);
	cv::imshow("disparity", disparity);
}

cv::Mat DisparityPipeline::readMatFromTxt(std::string filename, int rows, int cols) {
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