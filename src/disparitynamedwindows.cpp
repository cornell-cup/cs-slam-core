#include "disparitynamedwindows.h"

DisparityNamedWindows* DisparityNamedWindows::_instance;

// singleton for the handler
void DisparityNamedWindows::initialize(StereoCamera* camera) {
  if(!_instance) {
    _instance = new DisparityNamedWindows(camera);
  }
}

DisparityNamedWindows::~DisparityNamedWindows(){}

// set the callback with this singleton object's static method
DisparityNamedWindows::DisparityNamedWindows(StereoCamera* camera) {
  _camera = camera;

  cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("disparity");

  cv::setMouseCallback("disparity", _mouseEventCallback, NULL);

  cv::namedWindow("2D Map");
}

// mouse callback to print helpful info
void DisparityNamedWindows::_mouseEventCallback(int event, int x, int y, int flags, void* userdata) {
     if  (event == cv::EVENT_LBUTTONDOWN ){
      int disp = _instance->_camera->getDispAt(y, x);
      int distance = reproject_utils::disparityToCm(disp);
      int w = _instance->_camera->getWidth();
      int h = _instance->_camera->getHeight();

      float x_proj = reproject_utils::reprojectX(x, disp, w);
      float y_proj = reproject_utils::reprojectY(y, disp, h);
      float z_proj = reproject_utils::reprojectZ(disp, 0.8f, w);
      std::cout << "Distance: " << distance << "cm, \t Disparity: " << disp << " \t at (" << x_proj << ", " << y_proj << "," << z_proj << ")" << std::endl;
     }
     //else if (event == cv::EVENT_MOUSEMOVE){
       //   std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
     //}
}

void DisparityNamedWindows::updateDisplay(FeatureTracker& featureTracker, Map2D& overhead) {
  cv::Mat vis = _instance->_camera->getLeftCamera()->getFrame()->clone();
  cv::Mat visDisp = _instance->_camera->getDisparityNorm()->clone();
  std::vector<cv::Point2f>* initFeatures = featureTracker.getInitFeatures();
  std::vector<cv::Point2f>* curFeatures = featureTracker.getCurFeatures();

  // draw tracking points on the left camera image
  for(int i = 0; i < initFeatures->size(); i++) {
    cv::Point2f curPoint = curFeatures->at(i);
    cv::Point2f startPoint = initFeatures->at(i);
    cv::line(vis, startPoint, curPoint, cv::Scalar(0, 128, 0));
    // cv::line(visDisp, startPoint, curPoint, cv::Scalar(0, 128, 0));
    cv::circle(vis, curPoint, 2, cv::Scalar(0,255,0), -1);
    // cv::circle(visDisp, curPoint, 2, cv::Scalar(0,255,0), -1);
  }

  cv::imshow("left", vis);
  cv::imshow("right", *(_instance->_camera->getRightCamera()->getFrame()));
  cv::imshow("disparity", visDisp);

  cv::imshow("2D Map", *(overhead.getVisual()));
}
