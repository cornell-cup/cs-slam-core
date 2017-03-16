#include "stereocamera.h"

StereoCamera::StereoCamera(int width, int height, int rate, int cam_idx_left, int cam_idx_right){
  _cameraLeft = cv::VideoCapture(cam_idx_left);
  _cameraLeft.set(CV_CAP_PROP_FRAME_WIDTH, widen);
  _cameraLeft.set(CV_CAP_PROP_FRAME_HEIGHT, height);
  _cameraLeft.set(CV_CAP_PROP_FPS, rate);

  _cameraRight = cv::VideoCapture(cam_idx_right);
  _cameraRight.set(CV_CAP_PROP_FRAME_WIDTH, width);
  _cameraRight.set(CV_CAP_PROP_FRAME_HEIGHT, height);
  _cameraRight.set(CV_CAP_PROP_FPS, rate);
}

StereoCamera::~StereoCamera() {}

void StereoCamera::getImage(cv::Mat& left_img, cv::Mat& right_img){
  _cameraLeft >> left_img;
  _cameraRight >> right_img;

}