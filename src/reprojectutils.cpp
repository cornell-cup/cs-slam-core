#include "reprojectutils.h"

namespace reproject_utils {

  cv::Mat getReprojectMat(unsigned int w, unsigned int h, float f) {
    // repojection matrix
    float data[4][4] = { { 1, 0, 0, -0.5f*w },
                         { 0,-1, 0,  0.5f*h },
                         { 0, 0, 0,     -f*w },
                         { 0, 0, 1,      0 } };

    return cv::Mat(4, 4, CV_32FC1, &data);
  }

  void reprojectArr(int x, int y, int disp, int h, int w, float* dest) {
    dest[0] = (x-0.5f*w)/ disp;
    dest[1] = (-y+0.5f*h)/ disp;
    dest[2] = (-0.8*w)/ disp;
  }

  float disparityToCm(int dispValue) {
    if (dispValue < 400 || dispValue > 1400) {
      return -1;
    }
    // inverse relationship * scalar
    return (1.f / dispValue)*108600.f;
  }
}