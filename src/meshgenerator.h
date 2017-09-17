#ifndef _SLAM_MESH_GENERATOR
#define _SLAM_MESH_GENERATOR

#include <opencv2/core/core.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>

struct Point3D {
  int x;
  int y;
  int z;
};

struct PointRC {
  int r;
  int c;
};

struct TriangleMesh {
  PointRC p1;
  PointRC p2;
  PointRC p3;
};

struct Mesh {
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

  cv::Mat _meshIdx;
  cv::Mat _pointIdx;
  cv::Mat _visited;

  void _iterateNewMesh(
    int id, 
    PointRC first_point,
    cv::Mat *input,
    std::queue<PointRC>& unassigned_mesh_edge_queue
  );

  void _fillResolution32(cv::Mat& mat, int r, int c, int v);
  void _fillResolution8(cv::Mat& mat, int r, int c, int v);
  bool _pointInBounds(int r, int c, int h, int w);
};

#endif
