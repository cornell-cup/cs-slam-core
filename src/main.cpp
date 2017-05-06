#include <string>

#include "disparitypipeline.h"
#include "meshgenerator.h"
#include "mousehandler.h"
#include "map2d.h"

#include <thread>
#include <mutex>
#include <chrono>

#define _USE_FILES

cv::Mat* disp_ptr;
std::mutex disp_ptr_lock;

void vision_loop() {
	// initialize the cameras to be used (either from files or physical cameras)
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

	// initialize the disparity pipeline with the cameras and the intial nudge amount
	DisparityPipeline pipeline = DisparityPipeline(leftCamera, rightCamera, 0);
	disp_ptr = pipeline.getDisparity();

	// setup the mousehandler for OpenCV image mouse events
	MouseHandler::initialize(&pipeline);

	// initalize the mesh generator object
	MeshGenerator meshGenerator = MeshGenerator();

	// set to true to quit the loop
	int quit = 0;

	// set to 1 to save a mesh once
	int savedMesh = 0;

	// initialize the overhead map object
	Map2D map2d;

	while (!quit){
		std::cout << "much vision" << std::endl;
		// mutex for disp map
		std::lock_guard<std::mutex>* lock = new std::lock_guard<std::mutex>(disp_ptr_lock);

		// process the next frame from the camera in the disparity pipeline
		pipeline.nextFrame();

		// release the lock
		delete lock;
 
		// display the camera frames and the normalized disparity map
		pipeline.updateDisplay();
		
		// generate meshes from the disparity map
		meshGenerator.generateMesh(pipeline.getDisparity());
		
		// update the overhead map and display the map
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
		else if (key == 115)	// w
			pipeline.nudge(1);
		else if (key == 119)	// s
			pipeline.nudge(-1);

		else if (savedMesh == 1) {
			meshGenerator.writeToFile("mesh2.ply", pipeline.getColorMat());
			savedMesh = 0;
		}
	}
}

void server_loop() {
	std::chrono::seconds sec(1);
	while(1){
		std::cout << "Looping" << std::endl;
		std::this_thread::sleep_for(sec);
	}
}

int main() {
	// cv::namedWindow("left");
	std::thread vision(vision_loop);
	std::thread server(server_loop);
	
	vision.join();
	server.join();
	// vision_loop();

	return 0;
}
