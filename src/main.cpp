#include <string>

#include "disparitypipeline.h"
#include "meshgenerator.h"
#include "mousehandler.h"
#include "profiler.h"
#include "map2d.h"

#define _USE_FILES

int main() {
	std::cout << "starting program" << std::endl;

	#ifdef _USE_FILES
	OpenCVCamera leftCamera = OpenCVCamera("resources/right.mp4");
	#else
	OpenCVCamera leftCamera = OpenCVCamera(1);
	#endif

	leftCamera.configure(640, 480, 30);
	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");

	#ifdef _USE_FILES
	OpenCVCamera rightCamera = OpenCVCamera("resources/left.mp4");
	#else
	OpenCVCamera rightCamera = OpenCVCamera(0);
	#endif

	rightCamera.configure(640, 480, 30);
	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	DisparityPipeline pipeline = DisparityPipeline(leftCamera, rightCamera, -2);

	MouseHandler::initialize(&pipeline);

	MeshGenerator meshGenerator = MeshGenerator();

	Profiler loopTime;
	loopTime.start();

	// set to true to quit the loop
	int quit = 0;

	int savedMesh = 0;

	Map2D map2d;

	while (!quit){
		loopTime.split();
		pipeline.nextFrame();
 
		pipeline.updateDisplay();
		
		meshGenerator.generateMesh(pipeline.getDisparity());
		
		map2d.updateMap(*meshGenerator.getMeshes(), leftCamera.getWidth(), rightCamera.getHeight());
		map2d.displayMap();

		// check if the esc key has been pressed to exit the loop
		int key = cv::waitKey(1);
		if (key == 27) // esc
			quit = 1;
		else if (key == 112) {	// p
			meshGenerator.writeToFile("mesh.ply", pipeline.getColorMat());
			//pipeline.writePointCloud("points.ply");
			savedMesh = 1;
		}
		else if (key == 115)
			pipeline.nudge(1);
		else if (key == 119)
			pipeline.nudge(-1);

		else if (savedMesh == 1) {
			meshGenerator.writeToFile("mesh2.ply", pipeline.getColorMat());
			savedMesh = 0;
		}
	}
	return 0;
}
