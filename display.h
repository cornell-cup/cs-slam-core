#ifndef _SLAM_DISPLAY
#define _SLAM_DISPLAY

#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

class Display {
  private:
    std::vector<std::string> _displayNames;

  public:
    Display(std::vector<std::string> displayNames);
    virtual ~Display();

    void DisplayImages(std::vector<cv::Mat>& displayImages);
};

#endif