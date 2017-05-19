#include "featuretracker.h"

FeatureTracker::FeatureTracker() {
  _curTick = 0;
  _resetTickNum = 4;
  _backThreshold = 1.f;
}

FeatureTracker::~FeatureTracker() {

}

void FeatureTracker::trackFeatures(cv::Mat& img) {
  // convert to grayscale
  cv::Mat grayImg;
  cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);

  if(_curFeatures.empty()) {
    // continue
  } else {
    std::vector<cv::Point2f> updatedFeatures;
    std::vector<cv::Point2f> prevFeaturesRecalc;
    std::vector<uchar> flowStatus;

    // calculate optical flow from prev frame to cur frame
    _calcOpticalFlow(_prevGray, grayImg, _curFeatures, updatedFeatures, flowStatus);
    // calculate optical flow backwards
    std::vector<uchar> backFlowStatus;
    _calcOpticalFlow(grayImg, _prevGray, updatedFeatures, prevFeaturesRecalc, backFlowStatus);

    // update the features
    _curFeatures = updatedFeatures;

    /*
    // calculate discripency between prev and recalculated prev points
    cv::Mat diffMulti;
    cv::absdiff(_curFeatures, prevFeaturesRecalc, diffMulti);
    // split the two channels and calculate the max diff for each point
    cv::Mat diffSplit[2];
    cv::split(diffMulti, diffSplit);
    cv::Mat diff;
    cv::max(diffSplit[0], diffSplit[1], diff);

    // std::cout << diffMulti << std::endl;
    
    // set a status vector where 255 means the point is good and 0 means the point is bad
    cv::Mat diffStatus;
    cv::compare(diff, cv::Scalar(_backThreshold), diffStatus, cv::CMP_LT);
    */

    // std::cout << updatedFeatures.size() << "," << updatedFeatures.at<cv::Vec<float, 2>>(0,1) << "," << status.type() << std::endl;

    // update the points
    // _curFeatures = updatedFeatures[status].copy()
    // _initFeatures = _initFeatures[status].copy()
    // filterFeatures(status/255);

    // update the prev gray frame
    _prevGray = grayImg;
  }

  if(_curTick < _resetTickNum) {
    _curTick++;
  } else {
    _curTick = 0;

    // set the points to be tracked for the next resetTickNum ticks
    _findFeatures(grayImg, _initFeatures);
    _curFeatures = _initFeatures;
    _prevGray = grayImg;
  }


}

// filter where 0 means exclude and 1 means include
void FeatureTracker::filterFeatures(std::vector<cv::Point2f>& filter) {
  /*
  int oldSize = _curFeatures.rows;

  int newSize = (int) cv::sum(filter)[0];
  std::cout << newSize << std::endl;

  cv::Mat filteredInitFeatures(1, newSize, CV_32FC2);
  cv::Mat filteredCurFeatures(1, newSize, CV_32FC2);

  int curIdx = 0;
  for(int i = 0; i < oldSize; i++) {
    if(filter.at<char>(i,0) == 1) {
      filteredInitFeatures.at<cv::Vec<float, 2>>(curIdx,0) = _initFeatures.at<cv::Vec<float, 2>>(i,0);
      filteredCurFeatures.at<cv::Vec<float, 2>>(curIdx,0) = _curFeatures.at<cv::Vec<float, 2>>(i,0);
      curIdx++;
    }
  }

  _initFeatures = filteredInitFeatures;
  _curFeatures = filteredCurFeatures;
  */
}

std::vector<cv::Point2f>* FeatureTracker::getCurFeatures() {
  return &_curFeatures;
}

std::vector<cv::Point2f>* FeatureTracker::getInitFeatures() {
  return &_initFeatures;
}

void FeatureTracker::_findFeatures(cv::Mat& inputImg, std::vector<cv::Point2f>& dest) {
  /*
   * args:
   * input image (grayscale)
   * output corners
   * max corners
   * quality level
   * min distance
   * mask
   * block size
   */
  cv::goodFeaturesToTrack(inputImg, dest, 1000, 0.01, 8, cv::noArray(), 19);

  // TODO figure out if necessary
  // cv::Size subPixWinSize(10,10);
  // cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);
  /*
   * args:
   * image
   * current corner points
   * half of the window side length to search
   * zero zone, (-1,-1) means there is none
   * termination criteria
   */
  // cornerSubPix(inputImg, dest, subPixWinSize, cv::Size(-1,-1), termcrit);
}

void FeatureTracker::_calcOpticalFlow(cv::Mat& prevGray, cv::Mat& gray, std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& dest, std::vector<uchar>& status) {
  std::vector<float> err;

  cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 10, 0.03);
  cv::Size winSize(19,19);

  /*
   * args:
   * previous image
   * current image
   * previous point locations
   * new point locations
   * status of each point
   * error of each point
   * the search window size
   * number of pyramid levels to use for the algorithm
   * termination criteria of the algorithm
   */
  cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, dest, status, err, winSize, 2, termcrit);
}