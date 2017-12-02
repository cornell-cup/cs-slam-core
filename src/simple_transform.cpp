#include "simple_transform.h"

SimpleTransform::SimpleTransform() {
  _theta = 0;
  _x = 0;
  _y = 0;
}

SimpleTransform::~SimpleTransform() {}

void SimpleTransform::computeTransform(int w, int h, FeatureTracker& featureTracker) {
  float cent_x = w/2.f;
  float cent_y = h/2.f;

  // the average horizontal movement of the optical flow
  // used to compute delta theta
  float average_horizontal_movement = 0.f;

  // the average inward or outward movement of the optical flow
  // used to compute the forwards or backwards movement
  // which updates x and y based on theta
  float average_centripetal_movement = 0.f;

  // TODO take into account the offset of the cameras from the axis of rotation
  // like thats^ gonna help, lol

  // get the vectors of points
  std::vector<cv::Point2f>* initFeatures = featureTracker.getInitFeatures();
  std::vector<cv::Point2f>* curFeatures = featureTracker.getCurFeatures();

  int num_features = initFeatures->size();

  if (num_features > 0) {
    for(int i = 0; i < num_features; i++) {
      cv::Point2f curPoint = curFeatures->at(i);
      cv::Point2f startPoint = initFeatures->at(i);

      // compute the change in x and y from the initially tracked point
      float delta_x = curPoint.x - startPoint.x;
      float delta_y = curPoint.y - startPoint.y;

      average_horizontal_movement += delta_x;

      float centrip_vect_x = startPoint.x - cent_x;
      float centrip_vect_y = startPoint.y - cent_y;
      // compute dot product between centrip vector and delta vector
      float dot_p = centrip_vect_x*delta_x + centrip_vect_y*delta_y;

      // normalize vectors by distance from center
      float dist = sqrt(centrip_vect_x*centrip_vect_x + centrip_vect_y*centrip_vect_y);

      float mag = sqrt(delta_x*delta_x + delta_y*delta_y);

      float norm = dist*mag;
      dot_p = dot_p / norm;

      // vectors further away from center should have larger magnitudes when moving forward
      // maybe normalize by magnitude of vector

      average_centripetal_movement += dot_p;
    }

    average_horizontal_movement = average_horizontal_movement / num_features;
    average_centripetal_movement = average_centripetal_movement / num_features;

    printf("delta theta: %f\ndelta movement: %f\n", average_horizontal_movement, average_centripetal_movement);

    // TODO cherry pick points to match points in quadrants and make it so they balance out
    // because the algorithm requires balanced points
    // should be able to achieve this with current setup and sufficient number of points
    // but it could be made a bit better

    // TODO need to find these values
    // horizontal pixel movement to radians
    float theta_scalar = 0.1f;
    float dist_scalar = 1.f;

    float new_theta = _theta + theta_scalar*average_horizontal_movement;
    float delta_movement = dist_scalar*average_centripetal_movement;

    float x_dirn = cos(new_theta); 
    float y_dirn = sin(new_theta); 

    float new_x = _x + x_dirn * delta_movement;
    float new_y = _y + y_dirn * delta_movement;
  }
}

int SimpleTransform::get_x() {
  return _x;
}
int SimpleTransform::get_y() {
  return _y;
}
