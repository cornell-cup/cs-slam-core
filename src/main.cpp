#include <string>

#include "disparity-pipe.h"


int main() {
  DisparityPipeline::CameraConfig camConfig;
  camConfig.width = 640;
  camConfig.height = 480;
  camConfig.rate = 30;

  DisparityPipeline pipeline = DisparityPipeline(camConfig, 1, 0, "calibration_mats");

  // set to true to quit the loop
  int quit = 0;

  while (!quit){
    
    // execute the pipeline
    pipeline.executePipeline();

    // check if the esc key has been pressed to exit the loop
    int key = cv::waitKey(1);
    if (key == 27) quit = 1;
  }
  return 0;
}
