#include "camera.h"

StereoCamera::StereoCamera(){
  _cameraLeft = cv::VideoCapture(CAMERA_INDEX_LEFT);
  _cameraLeft.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  _cameraLeft.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
  _cameraLeft.set(CV_CAP_PROP_FPS, FRAME_RATE);

  _cameraRight = cv::VideoCapture(CAMERA_INDEX_RIGHT);
  _cameraRight.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  _cameraRight.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
  _cameraRight.set(CV_CAP_PROP_FPS, FRAME_RATE);
}

StereoCamera::~StereoCamera() {}

void StereoCamera::getImages(cv::Mat& left_img, cv::Mat& right_img){
  _cameraLeft >> left_img;
  _cameraRight >> right_img;

}