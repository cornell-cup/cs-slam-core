#ifndef _SLAM_REPROJECT_UTILS
#define _SLAM_REPROJECT_UTILS

#include <algorithm>

#include <opencv2/core/core.hpp>

namespace reproject_utils {

  cv::Mat getReprojectMat(unsigned int w, unsigned int h, float f);

  void reprojectArr(int x, int y, int disp, int h, int w, float* dest);

  float disparityToCm(int dispValue);

  inline float reprojectX(int x, int z, unsigned int w) {
    return (x-0.5f*w)/ z;
  }

  inline float reprojectY(int y, int z, unsigned int h) {
    return (-y+0.5f*h)/ z;
  }

  inline float reprojectZ(int z, float f, unsigned int w) {
    return (-f*w)/ z;
  }
}

#endif