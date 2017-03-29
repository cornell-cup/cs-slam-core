#ifndef _SLAM_DISPARITY_PILELINE
#define _SLAM_DISPARITY_PILELINE

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "stereocamera.h"
#include "opencvcamera.h"
#include "depthmap.h"

class DisparityPipeline {
public:
	DisparityPipeline(OpenCVCamera& leftCamera, OpenCVCamera& rightCamera);
	virtual ~DisparityPipeline();

	void nextFrame();

	cv::Mat* getDisparity();

	void writePointCloud(std::string fname);

	void updateDisplay();

	int getDepthAt(int r, int c);

	void setDisparityMouseCallback(void (*cbFunc)(int event, int x, int y, int flags, void* userdata));

	void nudge(int amount);
private:
	StereoCamera _camera;
	DepthMap _dpMap;

	cv::Mat translateImg(cv::Mat &img, int offsetx, int offsety);
	int _nudgeAmount;

	int _currentTime();

	cv::Mat _disparity;
	cv::Mat _disparity_norm;

	cv::Mat _left;
	cv::Mat _right;

	static const std::string _plyHeaderPt1;
	static const std::string _plyHeaderPt2;
};

#endif
