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
  StereoCamera camera = StereoCamera(640, 480, 30, 0, 1);

  // intialize the calibration pipeline step
  Calibration calib = Calibration();

  // intialize the depth map pipeline step
  DepthMap dpMap = DepthMap();

  // set up the disparity map
  cv::Mat disparity = cv::Mat::zeros(camera.getHeight(), camera.getWidth(), CV_8U);

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
    Util::toGrayscale(cap->left, leftG);
    Util::toGrayscale(cap->right, rightG);

    // undistort the images
    calib.undistortImages(leftG, rightG, cap->left, cap->right);

    // calculate the disprity map
    dpMap.getDisparity(leftG, rightG, disparity);

    // normalize the output
    Util::normalize(disparity, disparity);

    // display the images
	Displays->draw("Left", cap->left);
	Displays->draw("Right", cap->right);
	Displays->draw("Displarity", disparity);

    // check if the esc key has been pressed to exit the loop
    int key = cv::waitKey(1);
    if (key == 27) quit = 1;
  }
  return 0;
}
