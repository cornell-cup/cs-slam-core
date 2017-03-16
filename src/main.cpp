#include <vector>
#include <string>

#include "display.h"
#include "stereocamera.h"
#include "util.h"
#include "calibration.h"
#include "depthmap.h"
#include "constants.h"

int main() {
  // setup the stereo camera
  StereoCamera camera = StereoCamera(640, 480, 30, 1, 0);

  // initialize the display pipe to display the images
  Display displayPipe = Display(Consts->displays);

  // intialize the calibration pipeline step
  Calibration calib = Calibration();

  // intialize the depth map pipeline step
  DepthMap dpMap = DepthMap();

  // set up the disparity map
  cv::Mat disparity = cv::Mat::zeros(HEIGHT, WIDTH, CV_8U);

  // set to true to quit the loop
  int quit = 0;

  // initialize the matricies
  // TODO investigate removal
  cv::Mat left, right, leftG, rightG;
  while (!quit){
	
    // get the images and set them to grayscale
    camera.getImage(left, right);
    // TODO see if this only needs to be done once, not a major issue though
    Util::toGrayscale(left, leftG);
    Util::toGrayscale(right, rightG);

    // undistort the images
    calib.undistortImages(leftG, rightG, left, right);

    // calculate the disprity map
    dpMap.getDisparity(leftG, rightG, disparity);

    // normalize the output
    Util::normalize(disparity, disparity);

    // add the left, right, and disparity images to the display vector
    std::vector<cv::Mat> images;
    images.push_back(left);
    images.push_back(right);
    images.push_back(disparity);

    // display the images
    displayPipe.displayImages(images);

    // check if the esc key has been pressed to exit the loop
    int key = cv::waitKey(1);
    if (key == 27) quit = 1;
  }
  return 0;
}
