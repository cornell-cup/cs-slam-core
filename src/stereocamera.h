#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class StereoCamera {
  private:
    cv::VideoCapture _cameraLeft;
    cv::VideoCapture _cameraRight;
  public:
    StereoCamera(int width, int height, int rate, int cam_idx_left, int cam_idx_right);

	virtual ~StereoCamera();

    void getImage(cv::Mat& left_img, cv::Mat& right_img);
};

#endif