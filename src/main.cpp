#include <string>

#include "disparitypipeline.h"

int main() {
	std::cout << "starting program" << std::endl;

	OpenCVCamera leftCamera = OpenCVCamera(1);
	leftCamera.configure(640, 480, 30);
	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	OpenCVCamera rightCamera = OpenCVCamera(0);
	rightCamera.configure(640, 480, 30);
	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	DisparityPipeline pipeline = DisparityPipeline(leftCamera, rightCamera);

	// set to true to quit the loop
	int quit = 0;
	while (!quit){
		pipeline.nextFrame();
		
		pipeline.updateDisplay();

		// std::cout << ((frameEndTime - frameStartTime)/ 1000000.0) << std::endl;

		// check if the esc key has been pressed to exit the loop
		int key = cv::waitKey(1);
		if (key == 27) // esc
			quit = 1;
		else if (key == 112)	// p
			pipeline.writePointCloud("points.ply");
		else if (key == 115)
			pipeline.nudge(1);
		else if (key == 119)
			pipeline.nudge(-1);
	}
	return 0;
}
