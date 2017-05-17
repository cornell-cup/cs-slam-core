#ifndef _SLAM_NAME_WINDOWS
#define _SLAM_NAME_WINDOWS

#include "stereocamera.h"
#include "reprojectutils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class DisparityNamedWindows {

public:
  // singleton intiailizer
  static void initialize(StereoCamera* camera);

  // update the named windows
  static void updateDisplay();

  // destructor
  virtual ~DisparityNamedWindows();

private:
  // class constructor
  DisparityNamedWindows(StereoCamera* camera);
  // singleton instance
  static DisparityNamedWindows* _instance;

  // named windows callback function
  static void _mouseEventCallback(int event, int x, int y, int flags, void* userdata);

  // camera pointer to get window data
  StereoCamera* _camera;
};

#endif