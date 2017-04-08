#ifndef _SLAM_CAMERA
#define _SLAM_CAMERA

// camera class interface
class Camera {
public:
  virtual Camera();
  virtual ~Camera();

  virtual void capture(cv::Mat& dest);

  virtual unsigned int getHeight() const;
  virtual unsigned int getWidth() const;
  virtual unsigned int getFrameRate() const;
};

#endif
