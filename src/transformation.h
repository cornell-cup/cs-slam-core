#ifndef _SLAM_TRANSFORMATION
#define _SLAM_TRANSFORMATION

#include "stereocamera.h"
#include "meshgenerator.h"
#include "featuretracker.h"
#include "reprojectutils.h"

#include <vector>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

class Transformation {

public:
  Transformation();
  virtual ~Transformation();

  void computeTransform(StereoCamera& camera, MeshGenerator& meshGenerator, FeatureTracker& featureTracker);

  cv::Mat* getTransform();

private:
  cv::Mat _transformationMatrix;
  cv::Mat _tempTransform;
  
  void _projectFromPrev(cv::Mat disp, std::vector<cv::Point2f>* points2D, std::vector<cv::Point3f>& dest, int w, int h);

  void _normRotations(cv::Mat& mat4);

  void _estimateRigidTransform3D(std::vector<cv::Point3f>& p1, std::vector<cv::Point3f>& p2, cv::Mat& dest);

  cv::Mat _prevDisparity;
  cv::Mat _initDisparity;

};

#endif