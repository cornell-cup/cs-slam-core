#ifndef _SLAM_MAP_2D
#define _SLAM_MAP_2D

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "meshgenerator.h"
#include "reprojectutils.h"

#include <math.h>

class Map2D {
public:
  Map2D();
  virtual ~Map2D();

  void updateMap(std::vector<Mesh>& meshes, int img_width, int img_height);

  void displayMap();

private:
  // radius of block map
  const static int _mapDims = 25;
  // proper scaling factor when translating from block map to image
  int _scaler;

  void _plotPoint(int x, int y, int s, float v);

  void _addToMap(Point3D p, int w, int h, int cent_x, int cent_y);
  // multiplier for each x and z to scale to block map
  float _multiplier;

  float _getYWeight(float y);

  cv::Mat _map;
  cv::Mat _tempMap;
  cv::Mat _visualMat;

  std::vector<float> _yStops;
  std::vector<float> _yWeights;

  float _alpha;
  // scaler inversely proportional to the average number of meshes
  float _meshInverseScalar;
};

#endif
