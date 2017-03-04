#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_RATE 30
#define CAMERA_INDEX_LEFT 1
#define CAMERA_INDEX_RIGHT 0

class StereoCamera {
  private:
    cv::VideoCapture _cameraLeft;
    cv::VideoCapture _cameraRight;
  public:
    StereoCamera();
    ~StereoCamera();

    void getImage(cv::Mat& left_img, cv::Mat& right_img);
};

#endif