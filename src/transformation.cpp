#include "transformation.h"

Transformation::Transformation() {
  _transformationMatrix = cv::Mat::eye(4, 4, CV_64F);
  _tempTransform = cv::Mat::eye(4, 4, CV_64F);
}

Transformation::~Transformation() {}

void Transformation::computeTransform(StereoCamera& camera, MeshGenerator& meshGenerator, FeatureTracker& featureTracker) {
  int w = camera.getWidth();
  int h = camera.getHeight();
  
  if(featureTracker.getCurTick() == 0) {
    // std::cout << _tempTransform << std::endl;
    // std::cout << _transformationMatrix << std::endl;

    // push updates
    _transformationMatrix = _transformationMatrix * _tempTransform;

    // set init disparity
    _initDisparity = _prevDisparity;
  }

  std::vector<cv::Point3f> initPoints;
  // recalc init projections
  _projectFromPrev(_initDisparity, featureTracker.getInitFeatures(), initPoints, w, h);

  /*
  _prevDisparity = cv::Mat::zeros(h, w, camera.getDisparity()->type());
  int numMeshes = meshGenerator.getMeshes()->size();
  for(int i = 0; i < numMeshes; i++) {
    Mesh mesh = meshGenerator.getMeshes()->at(i);
    for(int j = 0; j < mesh.points.size(); j++) {
      Point3D point = mesh.points.at(j);
      _prevDisparity.at<short>(point.y, point.x) = point.z;
    }
  }
  */
  _prevDisparity = camera.getDisparity()->clone();

  std::vector<cv::Point3f> curPoints;

  _projectFromPrev(_prevDisparity, featureTracker.getCurFeatures(), curPoints, w, h);

  // make sure we have enough points
  if(initPoints.size() > 6) {

    std::vector<uchar> inliers;
    cv::Mat affine;

    /*
    * args:
    * first set of 3D points
    * second set of 3D points
    * output 3 by 4 affine transform matrix
    * output indicating the inliers from RANSAC
    * max RANSAC error
    * confidence value
    */
    cv::estimateAffine3D(initPoints, curPoints, affine, inliers, 3.0, 0.99);

    // add standard row
    cv::Mat row = cv::Mat::zeros(1, 4, CV_64F);
    row.at<double>(0,3) = 1;
    affine.push_back(row);

    _tempTransform = affine;
    
  }
}

cv::Mat* Transformation::getTransform() {
  return &_transformationMatrix;
}

void Transformation::_projectFromPrev(cv::Mat disp, std::vector<cv::Point2f>* points2D, std::vector<cv::Point3f>& dest, int w, int h) {
  dest.clear();

  for(int i = 0; i < points2D->size(); i++) {
    float x = points2D->at(i).x;
    float y = points2D->at(i).y;
    if(x < 0)
      x = 0;
    if(y < 0)
      y = 0;

    // TODO interpolate z value from 8 surrounding pixels via weighted sum on normalized squared distance
    int z = disp.at<short>((int) round(y), (int) round(x));
    // TODO test if z is valid and create filter
    dest.push_back(cv::Point3f(
      reproject_utils::reprojectX(x, z, w),
      reproject_utils::reprojectY(y, z, h),
      reproject_utils::reprojectZ(z, 0.8, w)
    ));
  }
}