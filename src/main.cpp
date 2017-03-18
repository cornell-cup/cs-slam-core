#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "stereocamera.h"
#include "opencvcamera.h"
#include "depthmap.h"

void pointCloud(cv::Mat& disparity) {
	int w = disparity.size().width;
	int h = disparity.size().height;
	float f = 0.8*w;


	float data[4][4] = {{1, 0, 0, -0.5*w},
						{0,-1, 0,  0.5*h},
						{0, 0, 0,     -f},
						{0, 0, 1,      0}};

	cv::Mat Q = cv::Mat(2, 5, CV_32FC1, &data);

	cv::Mat points;

	cv::reprojectImageTo3D(disparity, points, Q);
}

int main() {
	OpenCVCamera leftCamera = OpenCVCamera(1);
	leftCamera.configure(640, 480, 30);
	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	OpenCVCamera rightCamera = OpenCVCamera(0);
	rightCamera.configure(640, 480, 30);
	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	// setup the stereo camera
	StereoCamera camera = StereoCamera(leftCamera, rightCamera);

	// intialize the depth map pipeline step
	DepthMap dpMap = DepthMap();

	// setup windows
	cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("disparity");

	// set to true to quit the loop
	int quit = 0;

	// initialize the matricies
	// TODO investigate removal
	cv::Mat leftG, rightG;
	while (!quit){
	
	// get the images
	std::shared_ptr<StereoCamera::StereoCapture> cap = camera.getImage();
	cv::Mat disparity = cv::Mat::zeros(camera.getHeight(), camera.getWidth(), CV_8U);

	//check to see that images were actually captured
	if (!cap->isValid())
	{
		std::cout << "Invalid Capture" << std::endl;
		continue;
	}

	// TODO see if this only needs to be done once, not a major issue though
	cv::cvtColor(cap->left, leftG, cv::COLOR_BGR2GRAY);
	cv::cvtColor(cap->right, rightG, cv::COLOR_BGR2GRAY);

	// calculate the disprity map
	dpMap.getDisparity(leftG, rightG, disparity);

	// display the images
	cv::imshow("left", cap->left);
	cv::imshow("right", cap->right);
	cv::imshow("disparity", disparity);

	// check if the esc key has been pressed to exit the loop
	int key = cv::waitKey(1);
	if (key == 27) quit = 1;
	if (key == 70) pointCloud(disparity);
	}
	return 0;
}
