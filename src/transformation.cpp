#include "transformation.h"

Transformation::Transformation() {
  _transformationMatrix = cv::Mat::eye(4, 4, CV_32F);
  _tempTransform = cv::Mat::eye(4, 4, CV_32F);
}

Transformation::~Transformation() {}

void Transformation::computeTransform(StereoCamera& camera, MeshGenerator& meshGenerator, FeatureTracker& featureTracker) {
  int w = camera.getWidth();
  int h = camera.getHeight();
  
  if(featureTracker.getCurTick() == 0) {
    // std::cout << _tempTransform << std::endl;
    // std::cout << _transformationMatrix << std::endl;

    // _normRotations(_tempTransform);

    // push updates
    _transformationMatrix = _transformationMatrix * _tempTransform;

    // _normRotations(_transformationMatrix);

    // set init disparity
    _initDisparity = _prevDisparity;
  }

  std::vector<cv::Point3f> initPoints;
  // recalc init projections because size may have changed
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

    // std::vector<uchar> inliers;
    // cv::Mat affine;

    /*
    * args:
    * first set of 3D points
    * second set of 3D points
    * output 3 by 4 affine transform matrix
    * output indicating the inliers from RANSAC
    * max RANSAC error
    * confidence value
    */
    // cv::estimateAffine3D(initPoints, curPoints, affine, inliers, 3.0, 0.99);

    // add homogeneous row
    // cv::Mat row = cv::Mat::zeros(1, 4, CV_64F);
    // row.at<double>(0,3) = 1;
    // affine.push_back(row);

    // _tempTransform = affine;
    
    _estimateRigidTransform3D(initPoints, curPoints, _tempTransform);
    std::cout << _tempTransform << std::endl;
  }
}

void Transformation::_estimateRigidTransform3D(std::vector<cv::Point3f>& p1, std::vector<cv::Point3f>& p2, cv::Mat& dest) {
  if (p1.size() != p2.size()) {
    printf("Error, sizes dont match up: %lu != %lu", p1.size(), p2.size());
  }

  cv::Point3f centroid1(0,0,0);
  cv::Point3f centroid2(0,0,0);

  int numPoints = p1.size();

  // compute mean of p1 and p2 into centroid1 and centroi2 respectively
  for(int i = 0; i < numPoints; i++) {
    centroid1 += p1[i];
    centroid2 += p2[i];
    // centroid1.x += p1[i].x;
    // centroid1.y += p1[i].y;
    // centroid1.z += p1[i].z;
    // centroid2.x += p2[i].x;
    // centroid2.y += p2[i].y;
    // centroid2.z += p2[i].z;
  }
  centroid1 /= (double) numPoints;
  centroid2 /= (double) numPoints;
  // centroid1.x *= (1.f/p1.size());
  // centroid1.y *= (1.f/p1.size());
  // centroid1.z *= (1.f/p1.size());
  // centroid2.x *= (1.f/p1.size());
  // centroid2.y *= (1.f/p1.size());
  // centroid2.z *= (1.f/p1.size());

  // covariance matrix 
  cv::Mat cov = cv::Mat::zeros(3,3,CV_32F);

  for(int i = 0; i < p1.size(); i++) {
    cv::Point3f diff1 = p1[i]-centroid1;
    cv::Point3f diff2 = p2[i]-centroid2;
    cov.at<float>(0,0) += diff1.x*diff2.x;
    cov.at<float>(0,1) += diff1.x*diff2.y;
    cov.at<float>(0,2) += diff1.x*diff2.z;
    cov.at<float>(1,0) += diff1.y*diff2.x;
    cov.at<float>(1,1) += diff1.y*diff2.y;
    cov.at<float>(1,2) += diff1.y*diff2.z;
    cov.at<float>(2,0) += diff1.z*diff2.x;
    cov.at<float>(2,1) += diff1.z*diff2.y;
    cov.at<float>(2,2) += diff1.z*diff2.z;
  }

  cv::Mat S, U, Vt;
  cv::SVD::compute(cov, S, U, Vt, cv::SVD::FULL_UV);

  cv::Mat V, Ut;
  cv::transpose(Vt, V);
  cv::transpose(U, Ut);
  // TODO check if V*Ut or U*Vt
  cv::Mat R = V*Ut;

  // check if reflection case
  if(cv::determinant(R) < 0) {
    std::cout << "Reflection detected" << std::endl;
    V.at<float>(0,2) *= -1;
    V.at<float>(1,2) *= -1;
    V.at<float>(2,2) *= -1;
    R = V*Ut;
  }

  // t = -R*cent_1 + cent2
  cv::Point3f t = centroid2;
  t.x -= R.at<float>(0,0)*centroid1.x + R.at<float>(0,1)*centroid1.y + R.at<float>(0,2)*centroid1.z;
  t.y -= R.at<float>(1,0)*centroid1.x + R.at<float>(1,1)*centroid1.y + R.at<float>(1,2)*centroid1.z;
  t.z -= R.at<float>(2,0)*centroid1.x + R.at<float>(2,1)*centroid1.y + R.at<float>(2,2)*centroid1.z;

  // combine rotation and translation into 1 4x4 matrix
  // | r r r tx |
  // | r r r ty |
  // | r r r tz |
  // | 0 0 0 1  |
  dest.at<float>(0,0) = R.at<float>(0,0);
  dest.at<float>(0,1) = R.at<float>(0,1);
  dest.at<float>(0,2) = R.at<float>(0,2);
  dest.at<float>(1,0) = R.at<float>(1,0);
  dest.at<float>(1,1) = R.at<float>(1,1);
  dest.at<float>(1,2) = R.at<float>(1,2);
  dest.at<float>(2,0) = R.at<float>(2,0);
  dest.at<float>(2,1) = R.at<float>(2,1);
  dest.at<float>(2,2) = R.at<float>(2,2);

  dest.at<float>(0,3) = t.x;
  dest.at<float>(1,3) = t.y;
  dest.at<float>(2,3) = t.z;

  dest.at<float>(3,0) = 0.f;
  dest.at<float>(3,1) = 0.f;
  dest.at<float>(3,2) = 0.f;
  dest.at<float>(3,3) = 1.f;
}

void Transformation::_normRotations(cv::Mat& mat4) {
  // normalize upper 3x3 of temp
  double mag = std::sqrt(mat4.at<double>(0,0)*mat4.at<double>(0,0) + mat4.at<double>(1,0)*mat4.at<double>(1,0) + mat4.at<double>(2,0)*mat4.at<double>(2,0));
  mat4.at<double>(0,0) /= mag;
  mat4.at<double>(1,0) /= mag;
  mat4.at<double>(2,0) /= mag;
  mag = std::sqrt(mat4.at<double>(0,1)*mat4.at<double>(0,1) + mat4.at<double>(1,1)*mat4.at<double>(1,1) + mat4.at<double>(2,1)*mat4.at<double>(2,1));
  mat4.at<double>(0,1) /= mag;
  mat4.at<double>(1,1) /= mag;
  mat4.at<double>(2,1) /= mag;
  mag = std::sqrt(mat4.at<double>(0,2)*mat4.at<double>(0,2) + mat4.at<double>(1,2)*mat4.at<double>(1,2) + mat4.at<double>(2,2)*mat4.at<double>(2,2));
  mat4.at<double>(0,2) /= mag;
  mat4.at<double>(1,2) /= mag;
  mat4.at<double>(2,2) /= mag;
}

cv::Mat* Transformation::getTransform() {
  return &_transformationMatrix;
}

void Transformation::_projectFromPrev(cv::Mat& disp, std::vector<cv::Point2f>* points2D, std::vector<cv::Point3f>& dest, int w, int h) {
  dest.clear();

  int dispWidth = disp.cols;
  int dispHeight = disp.rows;

  for(int i = 0; i < points2D->size(); i++) {
    float x = points2D->at(i).x;
    float y = points2D->at(i).y;
    if(x < 0) x = 0;
    if(x >= dispWidth) x = dispWidth - 1;
    if(y < 0) y = 0;
    if (x >= dispHeight) y = dispHeight - 1;

    // TODO interpolate z value from 8 surrounding pixels via weighted sum on normalized squared distance
    int z = disp.at<short>((int) round(y), (int) round(x));
    // TODO test if z is valid and create filter
    // TODO filter features based on mesh generator results
    dest.push_back(cv::Point3f(
      reproject_utils::reprojectX(x, z, w),
      reproject_utils::reprojectY(y, z, h),
      reproject_utils::reprojectZ(z, 0.8, w)
    ));
  }
}