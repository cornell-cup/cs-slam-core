#include "depthmap.h"

DepthMap::DepthMap() {
	int SADWindowSize = 20;
	// disparity smoothness parameter 1 (slanted surfaces)
	int p1 = 8 * 1 * SADWindowSize * SADWindowSize;
	// disparity smoothenss parameter 2 (depth discontinuities), must have p2 > p1
	int p2 = 4 * p1;

	// minimum disparity value
	int minDisparity = 25;
	// max disparity - minDisparity, must be divisible by 16
	int numDisparities = 64;
	// odd number, block size to match disparity
	int blockSize = 9;
	// Max difference in the left-right disparity check
	int disp12MaxDiff = 1;
	// truncation range for prefiltered pixels
	int preFilterCap = 63;
	// how much the best match should "win" by
	int uniquenessRatio = 7;
	// max smooth disparity region size
	int speckleWindowSize = 50;
	// max disparity variation in connected components
	int speckleRange = 1;

	_stereoSGBM = cv::StereoSGBM::create(
		minDisparity,
		numDisparities,
		blockSize,
		p1,
		p2,
		disp12MaxDiff,
		preFilterCap,
		uniquenessRatio,
		speckleWindowSize,
		speckleRange
	);

	// numDisparities, SADWindowSize (size of blocks compared by algorithm)
	_stereoBM = cv::StereoBM::create(128, 21);
}

DepthMap::~DepthMap() {}

void DepthMap::getDisparity(cv::Mat& left_img, cv::Mat& right_img, cv::Mat& disp_dest, int type) {
	if(type == 0) {
		_stereoSGBM->compute(left_img, right_img, disp_dest);
	} else if (type == 1) {
		_stereoBM->compute(left_img, right_img, disp_dest);
	}
}
