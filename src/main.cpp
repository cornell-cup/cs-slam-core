#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "stereocamera.h"
#include "opencvcamera.h"
#include "depthmap.h"

// ply file headers
std::string plyHeaderPt1 = "ply\n"
"format ascii 1.0\n"
"element vertex ";

std::string plyHeaderPt2 = "property float x\n"
"property float y\n"
"property float z\n"
"property uchar red\n"
"property uchar green\n"
"property uchar blue\n"
"end_header\n";

void pointCloud(cv::Mat& disparity, cv::Mat& left) {
	int w = disparity.size().width;
	int h = disparity.size().height;
	float f = 0.8*w;


	float data[4][4] = {{1, 0, 0, -0.5f*w},
						{0,-1, 0,  0.5f*h},
						{0, 0, 0,     -f},
						{0, 0, 1,      0}};

	cv::Mat Q = cv::Mat(4, 4, CV_32FC1, &data);

	cv::Mat points;

	cv::reprojectImageTo3D(disparity, points, Q);

	points = points.reshape(1, w*h);
	cv::cvtColor(left, left, cv::COLOR_BGR2RGB);
	cv::Mat colors = left.reshape(1, w*h);

	std::ofstream myfile;
	myfile.open("points.ply");
	myfile << plyHeaderPt1 << points.size().height << "\n" << plyHeaderPt2 << std::endl;

	for (int i = 0; i < points.size().height; i++) {
		myfile << points.at<float>(i,0) << " " << points.at<float>(i, 1) << " " << points.at<float>(i, 2) << " " << colors.at<int>(i, 0) << " " << colors.at<int>(i, 1) << " " << colors.at<int>(i, 2) << std::endl;
	}

	myfile.close();

	std::cout << "generated point cloud" << std::endl;
}

int nudgeAmount = 0;
cv::Mat translateImg(cv::Mat &img, int offsetx, int offsety) {
	cv::Mat trans_mat = (cv::Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	cv::warpAffine(img, img, trans_mat, img.size());
	return trans_mat;
}

int currentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int main() {
	std::cout << "starting program" << std::endl;

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

	int frames = 0;
	int startTime = currentTime();

	// initialize the matricies
	// TODO investigate removal
	cv::Mat leftG, rightG;
	while (!quit){
		int frameStartTime = currentTime();
	
		// get the images
		std::shared_ptr<StereoCamera::StereoCapture> cap = camera.getImage();

		if(nudgeAmount != 0)
			translateImg(cap->left, 0, nudgeAmount);
	

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

		cv::Mat disparity_norm;
		cv::normalize(disparity, disparity_norm, 0, 255, cv::NORM_MINMAX, CV_8U);

		// display the images
		cv::imshow("left", cap->left);
		cv::imshow("right", cap->right);
		cv::imshow("disparity", disparity_norm);

		int frameEndTime = currentTime();
		frames++;

		// std::cout << ((frameEndTime - frameStartTime)/ 1000000.0) << std::endl;

		// check if the esc key has been pressed to exit the loop
		int key = cv::waitKey(1);
		if (key == 27) // esc
			quit = 1;
		else if (key == 112)	// p
			pointCloud(disparity, cap->left);
		else if (key == 115)
			nudgeAmount++;
		else if (key == 119)
			nudgeAmount--;
	}
	return 0;
}
