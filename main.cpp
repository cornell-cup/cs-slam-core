#include <vector>
#include <string>

#include "display.h"
#include "camera.h"

int main() {
  StereoCamera camera;
  std::vector<std::string> displayNames;
  displayNames.push_back("Left");
  displayNames.push_back("Right");
  // displayNames.push_back("Disparity");
  Display displayPipe(displayNames);

  cv::Mat left, right;
  std::vector<cv::Mat> images;
  images.push_back(left);
  images.push_back(right);

  int quit = 0;
  while (!quit){
    camera.getImage(left, right);
    displayPipe.displayImages(images);


    int key = cv::waitKey(1);
    if (key == 27) quit = 1;
  }
  return 0;
}