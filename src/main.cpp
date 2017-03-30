#include <string>

#include "disparitypipeline.h"
#include "meshgenerator.h"

std::function<int(int, int)> getDispValue;

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata) {
     if  (event == cv::EVENT_LBUTTONDOWN ){
          std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
					std::cout << "Disparity value: " << getDispValue(x,y) << std::endl;
     }
     else if  (event == cv::EVENT_RBUTTONDOWN){
          std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if  (event == cv::EVENT_MBUTTONDOWN){
          std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if (event == cv::EVENT_MOUSEMOVE){
          std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
     }
}



int main() {
	std::cout << "starting program" << std::endl;

	OpenCVCamera leftCamera = OpenCVCamera(1);
	leftCamera.configure(640, 480, 30);
	leftCamera.loadCalibration("calibration_mats/cam_mats_left", "calibration_mats/dist_coefs_left");
	OpenCVCamera rightCamera = OpenCVCamera(0);
	rightCamera.configure(640, 480, 30);
	rightCamera.loadCalibration("calibration_mats/cam_mats_right", "calibration_mats/dist_coefs_right");

	DisparityPipeline pipeline = DisparityPipeline(leftCamera, rightCamera);

	getDispValue = [&pipeline](int x, int y) -> int {
		return pipeline.getDepthAt(y,x);
	};

  // set the mouse callback
  pipeline.setDisparityMouseCallback(mouseCallBackFunc);

	MeshGenerator meshGenerator = MeshGenerator();

	std::vector<Mesh> meshes;

	// set to true to quit the loop
	int quit = 0;
	while (!quit){
		pipeline.nextFrame();

		pipeline.updateDisplay();

		meshes.clear();
		meshGenerator.generateMesh(*pipeline.getDisparity(), &meshes);

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
