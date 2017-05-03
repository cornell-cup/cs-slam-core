#ifndef _SLAM_DEPTH_MAP
#define _SLAM_DEPTH_MAP

#include <opencv2/calib3d/calib3d.hpp>

class DepthMap {
  private:
    cv::Ptr<cv::StereoSGBM> _stereoSGBM;
	cv::Ptr<cv::StereoBM> _stereoBM;

  public:
    DepthMap();
    virtual ~DepthMap();

    void getDisparity(cv::Mat& left_img, cv::Mat& right_img, cv::Mat& disp_dest);
};

#endif