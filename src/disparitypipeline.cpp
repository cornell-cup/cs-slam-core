#include "disparitypipeline.h"

const std::string DisparityPipeline::_plyHeaderPt1 = "ply\n"
"format ascii 1.0\n"
"element vertex ";

const std::string DisparityPipeline::_plyHeaderPt2 = "property float x\n"
"property float y\n"
"property float z\n"
"property uchar red\n"
"property uchar green\n"
"property uchar blue\n"
"end_header\n";

// disparity map does not produce values left of 90 pixels
const int leftCuttof = 90;

DisparityPipeline::DisparityPipeline(OpenCVCamera& leftCamera, OpenCVCamera& rightCamera, int initNudge): _camera(leftCamera, rightCamera) {

	// intialize the depth map pipeline step
	_dpMap = DepthMap();

	// initialize disparity and disparity norm
	_disparity = cv::Mat::zeros(_camera.getHeight(), _camera.getWidth(), CV_8U);
	_disparity_norm = cv::Mat::zeros(_camera.getHeight(), _camera.getWidth(), CV_8U);

	// setup windows
	// cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("disparity");
	_nudgeAmount = initNudge;
}

void DisparityPipeline::nextFrame() {
	// get the images
	_camera.getImage(_left, _right);

	// nudge the image up or down
	if (_nudgeAmount != 0)
		translateImg(_left, 0, _nudgeAmount);

	//check to see that images were actually captured
	if (_right.empty() || _left.empty())
	{
		std::cout << "Invalid Capture" << std::endl;
		return;
	}

	cv::Mat leftG, rightG;

	// TODO see if this only needs to be done once, not a major issue though
	cv::cvtColor(_left, leftG, cv::COLOR_BGR2GRAY);
	cv::cvtColor(_right, rightG, cv::COLOR_BGR2GRAY);
	
	// calculate the disprity map
	_dpMap.getDisparity(leftG, rightG, _disparity);
	
	// normalize the disparity map for display
	cv::normalize(_disparity, _disparity_norm, 0, 255, cv::NORM_MINMAX, CV_8U);
}

cv::Mat* DisparityPipeline::getLeftImg() {
	return &_left;
}

cv::Mat* DisparityPipeline::getRightImg() {
	return &_right;
}

cv::Mat* DisparityPipeline::getDisparity() {
	return &_disparity;
}

// returns depth in centimeters
float DisparityPipeline::getDistanceAt(int r, int c) {
	int value = getDepthAt(r, c);
	if (value < 400 || value > 1400) {
		return -1;
	}
	// inverse relationship * scalar
	return (1.f / value)*108600.f;
}

short DisparityPipeline::getDepthAt(int r, int c) {
	return _disparity.at<short>(r,c);
}

cv::Mat* DisparityPipeline::getDisparityNorm() {
	return &_disparity_norm;
}

// get the matrix that has the RGB value associated with each pixel
cv::Mat* DisparityPipeline::getColorMat() {
	return &_left;
}

unsigned char DisparityPipeline::getDepthAtNorm(int r, int c) {
	return _disparity_norm.at<unsigned char>(r, c);
}

void DisparityPipeline::setDisparityMouseCallback(void (*cbFunc)(int event, int x, int y, int flags, void* userdata)) {
	//set the callback function for any mouse event
 	cv::setMouseCallback("disparity", cbFunc, NULL);
}

void DisparityPipeline::writePointCloud(std::string fname) {
	int w = _disparity.size().width;
	int h = _disparity.size().height;
	float f = 0.8*w;

	// repojection matrix
	float data[4][4] = { { 1, 0, 0, -0.5f*w },
	{ 0,-1, 0,  0.5f*h },
	{ 0, 0, 0,     -f },
	{ 0, 0, 1,      0 } };

	cv::Mat Q = cv::Mat(4, 4, CV_32FC1, &data);

	cv::Mat points;

	cv::reprojectImageTo3D(_disparity, points, Q);

	points = points.reshape(1, w*h);
	cv::cvtColor(_left, _left, cv::COLOR_BGR2RGB);
	cv::Mat colors = _left.reshape(1, w*h);

	std::ofstream myfile;
	myfile.open(fname);
	myfile << _plyHeaderPt1 << points.size().height - (leftCuttof*h) << "\n" << _plyHeaderPt2 << std::endl;

	for (int i = 0; i < points.size().height; i++) {
		if (i%w >= leftCuttof) {
			myfile << points.at<float>(i, 0) << " " << points.at<float>(i, 1) << " " << points.at<float>(i, 2) << " " << colors.at<int>(i, 0) << " " << colors.at<int>(i, 1) << " " << colors.at<int>(i, 2) << std::endl;
		}
	}

	myfile.close();

	std::cout << "generated point cloud" << std::endl;
}

void DisparityPipeline::updateDisplay() {
	if (_left.empty() || _right.empty()) {
		std::cout << "no image" << std::endl;
	} else {
		// display the images
		cv::imshow("left", _left);
		cv::imshow("right", _right);
		cv::imshow("disparity", _disparity_norm);
	}
}

DisparityPipeline::~DisparityPipeline() {}

void DisparityPipeline::nudge(int amount) {
	_nudgeAmount += amount;
}

cv::Mat DisparityPipeline::translateImg(cv::Mat &img, int offsetx, int offsety) {
	cv::Mat trans_mat = (cv::Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	cv::warpAffine(img, img, trans_mat, img.size());
	return trans_mat;
}

unsigned int DisparityPipeline::getWidth() {
	return _camera.getWidth();
}

unsigned int DisparityPipeline::getHeight() {
	return _camera.getHeight();
}
