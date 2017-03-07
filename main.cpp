#include <vector>
#include <string>

#include "display.h"
#include "camera.h"
#include "util.h"
#include "calibration.h"
#include "depthmap.h"

int main() {
  StereoCamera camera;
  std::vector<std::string> displayNames;
  displayNames.push_back("Left");
  displayNames.push_back("Right");
  displayNames.push_back("Disparity");
  Display displayPipe = Display(displayNames);

  Calibration calib = Calibration();
  DepthMap dpMap = DepthMap();

  cv::Mat disparity = cv::Mat::zeros();

  int quit = 0;
  while (!quit){
	
	cv::Mat left, right;
    camera.getImage(left, right);
	std::vector<cv::Mat> images;
	Util::toGrayscale(left, left);
	Util::toGrayscale(left, left);
	calib.undistortImages(left, right, left, right);



	dpMap.getDisparity(left, right, disparity);

	Util::normalize(disparity, disparity);

	images.push_back(left);
	images.push_back(right);
    displayPipe.displayImages(images);


    int key = cv::waitKey(1);
    if (key == 27) quit = 1;
  }
  return 0;
}