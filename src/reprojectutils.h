#ifndef _SLAM_REPROJECT_UTILS
#define _SLAM_REPROJECT_UTILS

#include <algorithm>

#include <opencv2/core/core.hpp>

namespace reproject_utils {

  // return the reprojection matrix, f usually = 0.8
  cv::Mat getReprojectMat(unsigned int w, unsigned int h, float f);

  // reproject the values and place them into dest like so: [x_new, y_new, z_new]
  void reprojectArr(int x, int y, int disp, int h, int w, float* dest);

  // translate the disparity value to centimeters
  float disparityToCm(int dispValue);

  // inline x reprojection
  inline float reprojectX(int x, int z, unsigned int w) {
    return (x-0.5f*w)/ z;
  }

  // inline y reprojection
  inline float reprojectY(int y, int z, unsigned int h) {
    return (-y+0.5f*h)/ z;
  }

  // inline z reprojection
  inline float reprojectZ(int z, float f, unsigned int w) {
    return (-f*w)/ z;
  }
}

#endif