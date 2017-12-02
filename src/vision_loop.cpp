#include "vision_loop.h"

VisionLoop::VisionLoop() {
	cameraPtr = NULL;
}

VisionLoop::~VisionLoop() {}

#define _USE_FILES
#define INIT_NUDGE -2
// #define VERBOSE

// get the current time
int VisionLoop::_getCurentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void VisionLoop::vision_loop () {
  // initialize the cameras to be used (either from files or physical cameras)
	#ifdef _USE_FILES
	OpenCVCamera leftCamera = OpenCVCamera("resources/right.mp4", 640, 480, 30);
	#else
	OpenCVCamera leftCamera = OpenCVCamera(1, 640, 480, 30);
	#endif

	// leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	leftCamera.nudge(INIT_NUDGE);

	#ifdef _USE_FILES
	OpenCVCamera rightCamera = OpenCVCamera("resources/left.mp4", 640, 480, 30);
	#else
	OpenCVCamera rightCamera = OpenCVCamera(0, 640, 480, 30);
	#endif

	// rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	StereoCamera camera = StereoCamera(leftCamera, rightCamera);
	cameraPtr = &camera;
	// DisparityNamedWindows::initialize(&camera);

	// initialize the mesh generator object
	meshGenerator = MeshGenerator();

	// initialize the transformation calculator
	Transformation transform;

	// initialize the simple transformation calculator
	SimpleTransform simple_transform;

	// set to true to quit the loop
	int quit = 0;

	// set to 1 to save a mesh once
	int savedMesh = 0;

	int curTime = _getCurentTime();
	int prevTime = curTime;

	while (!quit){
		curTime = _getCurentTime();
		#ifdef VERBOSE
			std::cout << ((curTime - prevTime) / 1000000.f) << std::endl;
		#endif
		prevTime = curTime;

		mesh_lock.lock();
		// process the next frame from the camera in the disparity pipeline
		camera.nextFrame();
		
		// generate meshes from the disparity map
		meshGenerator.generateMesh(camera.getDisparity());
		
		// update the overhead map and display the map
		map2d.updateMap(*meshGenerator.getMeshes(), leftCamera.getWidth(), rightCamera.getHeight());

		featureTracker.trackFeatures(*(camera.getLeftCamera()->getFrame()));

		simple_transform.computeTransform(leftCamera.getWidth(), rightCamera.getHeight(), featureTracker);

		// transform.computeTransform(camera, meshGenerator, featureTracker);

		featureTracker.tick();

		// release the lock
		mesh_lock.unlock();

		// display the camera frames and the normalized disparity map
		// DisparityNamedWindows::updateDisplay(featureTracker, map2d);

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