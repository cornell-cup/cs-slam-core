#ifndef _SLAM_FEATURE_TRACKER
#define _SLAM_FEATURE_TRACKER

#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

class FeatureTracker {

public:
  FeatureTracker();
  virtual ~FeatureTracker();

  void trackFeatures(cv::Mat& img);
  void filterFeatures(std::vector<cv::Point2f>& filter);

  std::vector<cv::Point2f>* getCurFeatures();
  std::vector<cv::Point2f>* getInitFeatures();

private:
  std::vector<cv::Point2f> _curFeatures;
  std::vector<cv::Point2f> _initFeatures;

  cv::Mat _prevGray;

  cv::Mat _transformation;

  void _findFeatures(cv::Mat& inputImg, std::vector<cv::Point2f>& dest);
  void _calcOpticalFlow(cv::Mat& prevGray, cv::Mat& gray, std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& dest, std::vector<uchar>& status);

  // void filterVector(cv::Mat& mat, cv::Mat& filter);

  int _curTick;
  int _resetTickNum;

  float _backThreshold;

};

#endif