#ifndef _SLAM_OPENCVCAMERA
#define _SLAM_OPENCVCAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>

class OpenCVCamera
{
public:
	OpenCVCamera(unsigned int id);
	// destructor
	virtual ~OpenCVCamera();

	void capture(cv::Mat& dest);

	void configure(unsigned int frame_width, unsigned int frame_height, unsigned int frame_rate);

	void loadCalibration(std::string cam_mat, std::string dist_mat);

	unsigned int getHeight() const;
	unsigned int getWidth() const;
	unsigned int getFrameRate() const;
	unsigned int getAttribute(unsigned int id) const;
	
protected:
	int _id;


private:
	cv::VideoCapture _capture;

	cv::Mat _cameraMatrix;
	cv::Mat _distanceCoeff;

	cv::Mat _readMatFromTxt(std::string filename, int rows, int cols);
};

#endif