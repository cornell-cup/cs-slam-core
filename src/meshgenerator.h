#ifndef _SLAM_MESH_GENERATOR
#define _SLAM_MESH_GENERATOR

#include <opencv2/core/core.hpp>

#include <vector>

class MeshGenerator {
public:
  MeshGenerator();
  virtual ~MeshGenerator();

  struct mesh {
    std::vector<int[3]> points;
    std::vector<int[3]> faces;
  };

  void generateMesh(cv::Mat input, std::vector<mesh>* meshes);

private:
  int _minValue;
  int _resolution;
  int _diffThreshold;
};

#endif