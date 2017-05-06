#ifndef _SLAM_MAP_2D
#define _SLAM_MAP_2D

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "meshgenerator.h"

#include <math.h>

class Map2D {
public:
  Map2D();
  virtual ~Map2D();

  void updateMap(std::vector<Mesh>& meshes, int img_width, int img_height);

  void displayMap();

private:
  const static int _mapDims = 25;
  int _scaler;

  void _plotPoint(int x, int y, int s, float v);

  void _addToMap(Point3D p, int w, int h, int cent_x, int cent_y);
  float _multiplier;

  float _getYWeight(float y);

  cv::Mat _map;
  cv::Mat _tempMap;
  cv::Mat _visualMat;

  std::vector<float> _yStops;
  std::vector<float> _yWeights;

  float _alpha;
  float _meshInverseScalar;
};

#endif
