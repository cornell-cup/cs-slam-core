#include <vector>
#include <string>

#include "display.h"
#include "stereocamera.h"
#include "util.h"
#include "calibration.h"
#include "depthmap.h"
#include "constants.h"

int main() {
  StereoCamera camera;
  Display displayPipe = Display(Consts->displays);

  Calibration calib = Calibration();
  DepthMap dpMap = DepthMap();

  // cv::Mat disparity = cv::Mat::zeros(HEIGHT, WIDTH, CV_8U);
  cv::Mat disparity;

  int quit = 0;
  while (!quit){
	
	cv::Mat left, right, leftG, rightG;
    camera.getImage(left, right);
	std::vector<cv::Mat> images;
	Util::toGrayscale(left, leftG);
	Util::toGrayscale(right, rightG);

	calib.undistortImages(leftG, rightG, left, right);

	dpMap.getDisparity(left, right, disparity);

	Util::normalize(disparity, disparity);

	images.push_back(left);
	images.push_back(right);
	images.push_back(disparity);

    displayPipe.displayImages(images);


    int key = cv::waitKey(100);
    if (key == 27) quit = 1;
  }
  return 0;
}
