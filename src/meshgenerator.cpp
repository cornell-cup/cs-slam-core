#include "meshgenerator.h"

MeshGenerator::MeshGenerator() {
  _minValue = 0;
  _resolution = 2;
  _diffThreshold = 2;
}

MeshGenerator::~MeshGenerator() {}

void MeshGenerator::generateMesh(cv::Mat input, std::vector<mesh>* meshes) {
  int w = input.size().width;
	int h = input.size().height;

  cv::Mat meshIdx = cv::Mat::zeros(h, w,  CV_32S);

  int r,c;
  for(r = 0; r < h; r+=_resolution) {
    for(c = 0; c < w; c+=_resolution) {
      int value = input.at<int>(r,c);
      if(value > _minValue) {
        int matchIdx = 0;

        // check 4 surrounding points (0, -1), (-1,-1), (-1,0), (-1,1)
        if(c - _resolution >= 0 && input.at<int>(r, c - _resolution) - value < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r, c - _resolution);
        } else if(c - _resolution >= 0 && r - _resolution >= 0 && input.at<int>(r - _resolution, c - _resolution) - value < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c - _resolution);
        } else if(r - _resolution >= 0 && input.at<int>(r - _resolution) - value < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c);
        } else if(c + _resolution < w && r - _resolution >= 0 && input.at<int>(r - _resolution, c + _resolution) - value < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c + _resolution);
        }

        // new mesh
        if (matchIdx == 0) {
          
        } 
        // append to mesh
        else {

        }
      }
    }
  }
}