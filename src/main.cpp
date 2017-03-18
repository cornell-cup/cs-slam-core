#include <vector>
#include <string>

#include "display.h"
#include "stereocamera.h"
#include "util.h"
#include "calibration.h"
#include "depthmap.h"
#include "constants.h"

int main() {
  OpenCVCamera leftCamera = OpenCVCamera(0)
  leftCamera.configure(640, 480, 30)
  leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
  OpenCVCamera rightCamera = OpenCVCamera(1)
  rightCamera.configure(640, 480, 30)
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
  }
  return 0;
}
