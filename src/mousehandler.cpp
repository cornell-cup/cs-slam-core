#include "mousehandler.h"

MouseHandler* MouseHandler::_instance;

// singleton for the handler
MouseHandler* MouseHandler::initialize(DisparityPipeline* pipeline) {
  if(!_instance) {
    _instance = new MouseHandler(pipeline);
  }
  return _instance;
}

MouseHandler::~MouseHandler(){}

// set the callback with this singleton object's static method
MouseHandler::MouseHandler(DisparityPipeline* pipeline) {
  _pipe = pipeline;
  _pipe->setDisparityMouseCallback(_mouseEventCallback);
}

void MouseHandler::_mouseEventCallback(int event, int x, int y, int flags, void* userdata) {
     if  (event == cv::EVENT_LBUTTONDOWN ){
		 int value = _instance->_pipe->getDistanceAt(y, x);
		 int value_2 = _instance->_pipe->getDepthAt(y, x);
		 float x_proj = (x - 0.5f*_instance->_pipe->getWidth()) / value_2;
		 float y_proj = (-y + 0.5f*_instance->_pipe->getHeight()) / value_2;
		 float z_proj = (-0.8*_instance->_pipe->getWidth()) / value_2;
          //std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
			std::cout << "Distance: " << value << ", \t Disparity: " << value_2 << " \t at (" << x_proj << ", " << y_proj << "," << z_proj << ")" << std::endl;
     }
     //else if (event == cv::EVENT_MOUSEMOVE){
       //   std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
     //}
}