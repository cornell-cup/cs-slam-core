#include <string>

#include "stereocamera.h"
#include "meshgenerator.h"
#include "map2d.h"
#include "meshlabio.h"
#include "disparitynamedwindows.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include "pipes/NamedPipeServer.h"
#include "r2/R2Protocol.hpp"

// #define _USE_FILES 
#define INIT_NUDGE -2
//#define VERBOSE

std::mutex disp_mat_lock;
NamedPipeServer* server;

void compressImgMatrix(cv::Mat& img, vector<uchar>& buffer) {
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	cv::imencode("png", img, buffer, compression_params);
}

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
	else if (packet_id.compare("image") == 0)
	{
		//return left image
		server->send()
	}

}

int getCurentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void vision_loop() {
	// initialize the cameras to be used (either from files or physical cameras)
	#ifdef _USE_FILES
	OpenCVCamera leftCamera = OpenCVCamera("resources/right.mp4", 640, 480, 30);
	#else
	OpenCVCamera leftCamera = OpenCVCamera(1, 640, 480, 30);
	#endif

	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");

	#ifdef _USE_FILES
	OpenCVCamera rightCamera = OpenCVCamera("resources/left.mp4", 640, 480, 30);
	#else
	OpenCVCamera rightCamera = OpenCVCamera(0, 640, 480, 30);
	#endif

	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	StereoCamera camera = StereoCamera(leftCamera, rightCamera);
	DisparityNamedWindows::initialize(&camera);

	server->start();

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

		// mutex for disp mat
		disp_mat_lock.lock();

		// process the next frame from the camera in the disparity pipeline
		camera.nextFrame();

		// unlock
		disp_mat_lock.unlock();
 
		// display the camera frames and the normalized disparity map
		DisparityNamedWindows::updateDisplay();
		
		// generate meshes from the disparity map
		meshGenerator.generateMesh(camera.getDisparity());
		
		// update the overhead map and display the map
		map2d.updateMap(*meshGenerator.getMeshes(), leftCamera.getWidth(), rightCamera.getHeight());
		map2d.displayMap();

		// check if the esc key has been pressed to exit the loop
		int key = cv::waitKey(1);
		if (key == 27) // esc
			quit = 1;
		else if (key == 112) {	// p
			meshlabio::writeMeshes("mesh.ply", leftCamera.getFrame(), meshGenerator.getMeshes(), 0);
			//pipeline.writePointCloud("points.ply");
			savedMesh = 1;
		}
		else if (key == 115) {	// s
			leftCamera.nudge(1);
		}
		else if (key == 119) {	// w
			leftCamera.nudge(-1);
		}
		else if (savedMesh == 1) {
			meshlabio::writeMeshes("mesh2.ply", leftCamera.getFrame(), meshGenerator.getMeshes(), 1);
			savedMesh = 0;
		}
	}
}

int main() {

	server = NamedPipeServer::create("\\\\.\\pipe\\slam");
	server->setOnRequestCallback(onClientPipeRequest);
	vision_loop();
	delete server;


	return 0;
}
