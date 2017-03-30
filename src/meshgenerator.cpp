#include "meshgenerator.h"

MeshGenerator::MeshGenerator() {
  _minValue = 10;
  _resolution = 4;
  _diffThreshold = 4;
  _minPoints = 20;
}

MeshGenerator::~MeshGenerator() {}

void MeshGenerator::generateMesh(cv::Mat input, std::vector<Mesh>* meshes) {
  int w = input.size().width;
	int h = input.size().height;

  cv::Mat meshIdx = cv::Mat::zeros(h, w,  CV_32S);
  cv::Mat pointIdx = cv::Mat::zeros(h, w,  CV_32S);

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
          Mesh mesh = {
            std::vector<Point3D>(),
            std::vector<TriangleMesh>()
          };
          // add this point to the mesh (x,y,z)
          mesh.points.push_back({c, r, value});
          pointIdx.at<int>(r,c) = mesh.points.size()-1;

          meshes->push_back(mesh);
          meshIdx.at<int>(r,c) = meshes->size();
        } 
        // append to mesh
        else {
          Mesh mesh = (*meshes)[matchIdx-1];
          mesh.points.push_back({c, r, value});
          pointIdx.at<int>(r,c) = mesh.points.size()-1;

          int p1Idx = c - _resolution >= 0 ? meshIdx.at<int>(r,c - _resolution) : 0;
          int p2Idx = r - _resolution >= 0 ? meshIdx.at<int>(r - _resolution, c) : 0;
          int p3Idx = (c + _resolution < w && r - _resolution >= 0) ? meshIdx.at<int>(c + _resolution, r - _resolution) : 0;

          // create a face with left and top point
          if(matchIdx == p1Idx && matchIdx == p2Idx) {
            mesh.faces.push_back({pointIdx.at<int>(r,c), pointIdx.at<int>(r,c - _resolution), pointIdx.at<int>(r - _resolution,c)});
          }

          // create a face with top and top-left point
          if(matchIdx == p2Idx && matchIdx == p3Idx) {
            mesh.faces.push_back({pointIdx.at<int>(r,c), pointIdx.at<int>(r - _resolution,c), pointIdx.at<int>(r - _resolution,c + _resolution)});
          }

          meshIdx.at<int>(r,c) = matchIdx;
        }
      }
    }
  }

  // clean up small meshes
  for(r = 0; r < meshes->size(); r++) {
    if((*meshes)[r].points.size() < _minPoints) {
      meshes->erase(meshes->begin() + r);
      r--;
    }
  }
}