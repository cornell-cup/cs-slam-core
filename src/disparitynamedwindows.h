#ifndef _SLAM_NAME_WINDOWS
#define _SLAM_NAME_WINDOWS

#include "stereocamera.h"
#include "reprojectutils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class DisparityNamedWindows {

public:
  static DisparityNamedWindows* initialize(StereoCamera* camera);

  static DisparityNamedWindows* getInstance();

  static void updateDisplay();

  virtual ~DisparityNamedWindows();

private:
  DisparityNamedWindows(StereoCamera* camera);
  static DisparityNamedWindows* _instance;

  static void _mouseEventCallback(int event, int x, int y, int flags, void* userdata);

  StereoCamera* _camera;
};

#endif