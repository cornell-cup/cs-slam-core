#include "depthmap.h"

DepthMap::DepthMap() {
  int SADWindowSize = 20;
  int p1 = 8 * 1 * SADWindowSize * SADWindowSize;
  int p2 = 4 * p1;

  int minDisparity = 25,
  int numDisparities = 64,
  int blockSize = 9,
  int disp12MaxDiff = 1,
  int preFilterCap = 63,
  int uniquenessRatio = 7,
  int speckleWindowSize = 50,
  int speckleRange = 1

  _stereoSGBM = cv::StereoSGBM.create(
    minDisparity,
    numDisparities,
    blockSize,
    p1,
    p2,
    disp12MaxDiff,
    preFilterCap,
    uniquenessRatio,
    speckleWindowSize,
    speckleRange
  );
}

DepthMap::~DepthMap() {}

void DepthMap::getDisparity(cv::Mat& left_img, cv::Mat& right_img, cv::Mat& disp_dest) {
  _stereoSGBM.compute(left_img, right_img, disp_dest);
}