#ifndef _SLAM_MAP_2D
#define _SLAM_MAP_2D

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "meshgenerator.h"

#include <math.h>

class Map2D {
public:
  Map2D(int w, int h);
  virtual ~Map2D();

  void updateMap(std::vector<Mesh>& meshes, int img_width, int img_height);

  void displayMap();

private:
  void _plotPoint(int x, int y, float v);

  void _addToMap(Point3D p, int h, int w);

  int _cent_x;
  int _cent_y;

  float _weight_per_mesh_point;
  float _multiplier;

  cv::Mat _map;
};

#endif
