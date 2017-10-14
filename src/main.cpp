#include <string>

#include "stereocamera.h"
#include "meshgenerator.h"
#include "map2d.h"
#include "meshlabio.h"
#include "disparitynamedwindows.h"
#include "featuretracker.h"
#include "transformation.h"

#include "server/server.h"

#include <mutex>
#include <chrono>
#include <vector>

#ifdef SLAM_PRODUCTION
#include "pipes/NamedPipeServer.h"
#include "r2/R2Protocol.hpp"
#endif

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

void printinfo(cv::Mat& M) {
	std::string ty =  type2str( M.type() );
	printf("Matrix: %s %dx%d \n", ty.c_str(), M.cols, M.rows );
}

typedef struct vision_state_t {
	cv::Mat rgb_left;					// 8U C3 width x height
	cv::Mat rgb_right;				// 8U C3 width x height
	cv::Mat grayscale_left;		// 8U C1 width x height
	cv::Mat grayscale_right;	// 8U C1 width x height
	cv::Mat disparity;				// 16S C1 width x height, depth map
	cv::Mat mesh_points;			// 8U C1 width x height, binary values representing filtered points
	cv::Mat mesh_meshes;			// 32S C1 width x height, mesh index at each point
} vision_state_t;

#define _USE_FILES
#define INIT_NUDGE -40
//#define VERBOSE

// mutex for the stereo matrix
std::mutex disp_mat_lock;

Server* transformServer;

#ifdef SLAM_PRODUCTION
// named pipe server
NamedPipeServer* server;

// compress mat to bytestream
void compressImgMatrix(cv::Mat& img, vector<uchar>& buffer) {
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	cv::imencode("png", img, buffer, compression_params);
}

void getResponsePacket(std::string request_id, R2Protocol::Packet& response_packet, std::string destination)
{
	response_packet.destination = destination;
	response_packet.source = "VISION";
	response_packet.id = "disparity_map";

	if (request_id.compare("disparity_map") == 0)
	{
		// mutex for disp map
		disp_mat_lock.lock();

		uchar* disparity_data = disp_ptr->data;
		size_t disparty_size = disp_ptr->step[0] * disp_ptr->rows;

		response_packet.data.insert(response_packet.data.begin(), disparity_data, disparity_data + disparty_size);
		disp_mat_lock.lock();
	}
	else if (request_id.compare("features") == 0)
	{
		//return features
	}
	else if (request_id.compare("overhead") == 0)
	{
		//return overhead
	}
	else if (packet_id.compare("image") == 0)
	{
		//return left image
		server->send();
	}
}

unsigned char* onClientPipeRequest(unsigned char * request, unsigned int* reply_size, size_t request_size)
{
	R2Protocol::Packet request_packet = {};
	std::vector<unsigned char> request_vector;
	request_vector.insert(request_vector.end(), request, request + request_size);
	R2Protocol::decode(request_vector, request_packet);

	std::string packet_id = std::string(request_packet.id);
	R2Protocol::Packet response_packet;
	getResponsePacket(packet_id, response_packet, request_packet.source);

	std::vector<uint8_t> response_bytes_vector;
	R2Protocol::encode(response_packet, response_bytes_vector);

	uint8_t* response_bytes = new uint8_t[response_bytes_vector.size()];


	*reply_size = response_bytes_vector.size();

	for (int i = 0; i < *reply_size; i++)
	{
		response_bytes[i] = response_bytes_vector.at(i);
	}

	return response_bytes;
}

#endif

// get the current time
int getCurentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void vision_loop() {
	// initialize the cameras to be used (either from files or physical cameras)
	#ifdef _USE_FILES
	OpenCVCamera leftCamera = OpenCVCamera("resources/right_trans.mp4", 640, 480, 30);
	#else
	OpenCVCamera leftCamera = OpenCVCamera(1, 640, 480, 30);
	#endif

	// leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	leftCamera.nudge(INIT_NUDGE);

	#ifdef _USE_FILES
	OpenCVCamera rightCamera = OpenCVCamera("resources/left_trans.mp4", 640, 480, 30);
	#else
	OpenCVCamera rightCamera = OpenCVCamera(0, 640, 480, 30);
	#endif

	// rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	StereoCamera camera = StereoCamera(leftCamera, rightCamera);
	DisparityNamedWindows::initialize(&camera);

	#ifdef SLAM_PRODUCTION
	server->start();
	#endif

	// initialize the mesh generator object
	MeshGenerator meshGenerator = MeshGenerator();

	// initialize the feature tracker
	FeatureTracker featureTracker;

	// initialize the transformation calculator
	Transformation transform;

	transformServer = new Server(&transform);
	transformServer->runServerThread();

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

		disp_mat_lock.lock();

		// process the next frame from the camera in the disparity pipeline
		camera.nextFrame();

		// release the lock
		disp_mat_lock.unlock();
 
		#ifdef SLAM_PRODUCTION
		// display the camera frames and the normalized disparity map
		pipeline.updateDisplay();
		#endif
		
		// generate meshes from the disparity map
		meshGenerator.generateMesh(camera.getDisparity());
		
		// update the overhead map and display the map
		map2d.updateMap(*meshGenerator.getMeshes(), leftCamera.getWidth(), rightCamera.getHeight());

		featureTracker.trackFeatures(*(camera.getLeftCamera()->getFrame()));

		// transform.computeTransform(camera, meshGenerator, featureTracker);

		cv::Mat leftG;
		cv::cvtColor(*(camera.getLeftCamera()->getFrame()), leftG, cv::COLOR_BGR2GRAY);
		printinfo(leftG);

		featureTracker.tick();

		// display the camera frames and the normalized disparity map
		DisparityNamedWindows::updateDisplay(featureTracker, map2d);

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

	#ifdef SLAM_PRODUCTION
	server = NamedPipeServer::create("\\\\.\\pipe\\slam");
	server->setOnRequestCallback(onClientPipeRequest);
	#endif

	vision_loop();

	#ifdef SLAM_PRODUCTION
	delete server;
	#endif


	return 0;
}
