#ifndef _SLAM_MESH_GENERATOR
#define _SLAM_MESH_GENERATOR

#include <opencv2/core/core.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

struct Point3D {
  int x;
  int y;
  int z;
};

struct TriangleMesh {
  int p1;
  int p2;
  int p3;
};

struct Mesh {
  std::vector<Point3D> points;
  std::vector<TriangleMesh> faces;
};

class MeshGenerator {
public:
  MeshGenerator();
  virtual ~MeshGenerator();

  void generateMesh(cv::Mat* input);

  std::vector<Mesh>* getMeshes();

private:
  int _minValue;
  int _resolution;
  int _diffThreshold;
  int _minMeshes;

  int _clipXLeft;

  std::vector<Mesh> _meshes;
};

#endif
