#include "display.h"

Display::Display(std::vector<std::string> displayNames) {
  _displayNames = displayNames;

  for(int i = 0; i < _displayNames.size(); i++) {
    cv::namedWindow(_displayNames.at(i));
  }
  
}

Display::~Display() {}

void Display::displayImages(std::vector<cv::Mat>& displayImages){
  if(displayImages.size() == _displayNames.size()) {
    for(int i = 0; i <  displayImages.size(); i++) {
      cv::imshow(_displayNames.at(i), displayImages.at(i));
    }
  } else {
    std::cout << "Incorrect image matrix dimensions\n";
  }
}