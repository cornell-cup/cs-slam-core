#include <string>

#include "disparitypipeline.h"
#include "meshgenerator.h"
#include "mousehandler.h"
#include "profiler.h"

int main() {
	std::cout << "starting program" << std::endl;

	PhysicalCamera leftCamera = PhysicalCamera(1);
	leftCamera.configure(640, 480, 30);
	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	PhysicalCamera rightCamera = PhysicalCamera(0);
	rightCamera.configure(640, 480, 30);
	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	DisparityPipeline pipeline = DisparityPipeline(&leftCamera, &rightCamera);

	MouseHandler::initialize(&pipeline);

	MeshGenerator meshGenerator = MeshGenerator();

	Profiler loopTime;
	loopTime.start();

	// set to true to quit the loop
	int quit = 0;
	while (!quit){
		loopTime.split();
		pipeline.nextFrame();

		pipeline.updateDisplay();

		meshGenerator.generateMesh(pipeline.getDisparity());

		// check if the esc key has been pressed to exit the loop
		int key = cv::waitKey(1);
		if (key == 27) // esc
			quit = 1;
		else if (key == 112)	// p
			meshGenerator.writeToFile("mesh.ply", pipeline.getColorMat());
			//pipeline.writePointCloud("points.ply");
		else if (key == 115)
			pipeline.nudge(1);
		else if (key == 119)
			pipeline.nudge(-1);
	}
	return 0;
}
