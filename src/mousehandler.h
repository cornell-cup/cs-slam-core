#ifndef _SLAM_MOUSE_HANDLER
#define _SLAM_MOUSE_HANDLER

#include "disparitypipeline.h"

class MouseHandler {
public:
  static MouseHandler* initialize(DisparityPipeline* pipeline);

  virtual ~MouseHandler();

private:
  MouseHandler(DisparityPipeline* pipeline);
  static MouseHandler* _instance;

  static void _mouseEventCallback(int event, int x, int y, int flags, void* userdata);

  DisparityPipeline* _pipe;
};

#endif