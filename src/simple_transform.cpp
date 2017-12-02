#include "simple_transform.h"

SimpleTransform::SimpleTransform() {
  _theta = 0;
  _x = 0;
  _y = 0;
}

SimpleTransform::~SimpleTransform() {}

void SimpleTransform::computeTransform(int w, int h, FeatureTracker& featureTracker) {
  
}

int SimpleTransform::get_x() {
  return _x;
}
int SimpleTransform::get_y() {
  return _y;
}
