#include <string>

#include "disparitypipeline.h"
#include "meshgenerator.h"
#include "mousehandler.h"
#include "map2d.h"
#include "pipes/NamedPipeServer.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include "r2/R2Protocol.hpp"

//#define _USE_FILES
#define INIT_NUDGE -2
//#define VERBOSE

cv::Mat* disp_ptr;
std::mutex disp_ptr_lock;
NamedPipeServer* server;

unsigned char* onClientPipeRequest(unsigned char * request, unsigned int* reply_size, size_t request_size)
{
	R2Protocol::Packet packet = {};
	std::vector<unsigned char> request_vector;
	request_vector.insert(request_vector.end(), request, request + request_size);
	R2Protocol::decode(request_vector, packet);

	std::string packet_id = std::string(packet.id);

	if (packet_id.compare("disparity_map") == 0)
	{
		//return disparity map
	}
	else if (packet_id.compare("features") == 0)
	{
		//return features
	}
	else if (packet_id.compare("overhead") == 0)
	{
		//return overhead
	}

}

int getCurentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

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
	DisparityPipeline pipeline = DisparityPipeline(leftCamera, rightCamera, INIT_NUDGE);
	disp_ptr = pipeline.getDisparity();
	server->start();
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

	int curTime = getCurentTime();
	int prevTime = curTime;

	while (!quit){
		curTime = getCurentTime();
		#ifdef VERBOSE
			std::cout << ((curTime - prevTime) / 1000000.f) << std::endl;
		#endif
		prevTime = curTime;

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
		else if (key == 115)	// s
			pipeline.nudge(1);
		else if (key == 119)	// w
			pipeline.nudge(-1);

		else if (savedMesh == 1) {
			meshGenerator.writeToFile("mesh2.ply", pipeline.getColorMat());
			savedMesh = 0;
		}
	}
}

int main() {
	// cv::namedWindow("left");

	server = NamedPipeServer::create("\\\\.\\pipe\\slam");
	server->setOnRequestCallback(onClientPipeRequest);
	vision_loop();
	delete server;


	return 0;
}
