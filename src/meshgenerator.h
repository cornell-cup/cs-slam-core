#ifndef _SLAM_MESH_GENERATOR
#define _SLAM_MESH_GENERATOR

#include <opencv2/core/core.hpp>

#include <vector>

class MeshGenerator() {
public:
  MeshGenerator(int minValue);
  virtual ~MeshGenerator();

  generateMesh(cv::Mat input, std::vector<int[3]>* points, std::vector<int[3]>* faces);
};

#endif