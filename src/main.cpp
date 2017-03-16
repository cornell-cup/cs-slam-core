#include <string>

#include "disparity-pipe.h"

int main() {
	DisparityPipeline pipeline = DisparityPipeline(
		640,						// width
		480,						// height
		30,							// frame rate
		1,							// left camera index
		0,							// right camera index		
		"calibration_mats"			// camera matricies root directory
	);

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
