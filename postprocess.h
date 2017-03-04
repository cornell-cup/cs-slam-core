#ifndef _SLAM_POST_PROCESS
#define _SLAM_POST_PROCESS

class PostProcess {
  public:
    PostProcess();
    virtual ~PostProcess();

    void normalize(cv::Mat& input, cv::Mat& output);
}

#endif