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
  std::vector<Point3D> points;
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

  // using [first_point] as the first point of a mesh, flood fill 
  // [input] to determine other points in the mesh with id [id]
  void _iterateNewMesh(
    int id, 
    PointRC first_point,
    cv::Mat *input,
    std::queue<PointRC>& unassigned_mesh_edge_queue
  );

  // fill meshIdx from (r,c) to (r+_resolution, +resolution) with id
  void _fillMesh(int r, int c, int id);

  // set point (r,c) to visited
  void _visit(int r, int c);

  // return true if point (r,c) has not been visited
  bool _not_visited(int r, int c);

  // return true if the point is in bounds, (r,c) is treated
  // like a box from (r,c) to (r+_resolution, +resolution) and
  // makes sure entire box is in bounds
  bool _pointInBounds(int r, int c, int h, int w);

  int _get_depth_at(cv::Mat* input, int r, int c);
};

#endif
