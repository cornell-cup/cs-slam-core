#ifndef _SLAM_OPENCVCAMERA
#define _SLAM_OPENCVCAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>

class OpenCVCamera
{
public:
	// constructor from live camera
	OpenCVCamera(unsigned int id, unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);
	// constructor from file
	OpenCVCamera(std::string filename, unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);

	// destructor
	virtual ~OpenCVCamera();

	// get the next frame
	void nextFrame();

	cv::Mat* getFrame();

	// translate the frame vertically by the given amount
	void nudge(int amount);

	// load the calibraton matricies and initialize the calibration
	void loadCalibration(std::string cam_mat, std::string dist_mat);

	// read camera constants
	unsigned int getHeight() const;
	unsigned int getWidth() const;
	unsigned int getFrameRate() const;
	unsigned int getAttribute(unsigned int id) const;

private:
	// the CV video capture object
	cv::VideoCapture _capture;

	// the most recent frame captured by this camera
	cv::Mat _frame;

	// the current translation value
	int _nudgeAmount;
	// translate an the input image
	cv::Mat _translateImg(cv::Mat &img, int offsetx, int offsety);

	// set the necessary parameters for the capture object
	void _configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);

	// calibraton matricies
	cv::Mat _cameraMatrix;
	cv::Mat _distanceCoeff;

	// read a matrix from a text file
	cv::Mat _readMatFromTxt(std::string filename, int rows, int cols);

	// calibration matrix vectors
	std::vector<cv::Mat> _map1;
	std::vector<cv::Mat> _map2;

	// set the calibration maps
	void _setMaps(cv::Mat& _src);
};

#endif