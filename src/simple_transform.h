#ifndef _SLAM_SIMPLE_TRANSFORM
#define _SLAM_SIMPLE_TRANSFORM

#include <iostream>
#include <vector>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

#include "featuretracker.h"
#include "reprojectutils.h"

class SimpleTransform {

public:
  SimpleTransform();
  virtual ~SimpleTransform();

  void computeTransform(int w, int h, FeatureTracker& featureTracker);

  int get_x();
  int get_y();

private:
  float _theta;
  float _x;
  float _y;

};

#endif