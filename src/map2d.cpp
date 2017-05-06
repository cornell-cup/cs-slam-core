#include "map2d.h"

// half of the dimension of the visualiation mat
const int Map2DSize = 300;

Map2D::Map2D() {
  cv::namedWindow("2D Map");

  // multiplier for -1 -> 1 projection to array rows/cols
  _multiplier = _mapDims*0.75f;

  // scale for the visualization mat
  _scaler = Map2DSize/_mapDims;

  // increase by mesh resolution/count to inversely scale the added belief per pixel
  _meshInverseScalar = 15.f;

  _map = cv::Mat::zeros(_mapDims*2,_mapDims*2, CV_32F);
  _visualMat = cv::Mat::zeros(Map2DSize*2,Map2DSize*2, CV_32F);

  // y ~= [-0.6,0.6]
  // maybe add +-0.05

  // initialize the y stops and weights
  _yStops.push_back(-1.f);
  _yStops.push_back(0.3f);

  _yWeights.push_back(1.f);
  _yWeights.push_back(1.f);
  _yWeights.push_back(1.f);

  // low pass filter parameter
  _alpha = 0.1f;
}

Map2D::~Map2D() {}

void Map2D::updateMap(std::vector<Mesh>& meshes, int img_width, int img_height) {
  _tempMap = cv::Mat::zeros(_mapDims*2,_mapDims*2, CV_32F);

  // add all mesh points to the array
  for (int m = 0; m < meshes.size(); m++) {
    for (int f = 0; f < meshes[m].faces.size(); f++) {
      _addToMap(meshes[m].points[meshes[m].faces[f].p1], img_width, img_height, _mapDims, _mapDims);
      _addToMap(meshes[m].points[meshes[m].faces[f].p2], img_width, img_height, _mapDims, _mapDims);
      _addToMap(meshes[m].points[meshes[m].faces[f].p3], img_width, img_height, _mapDims, _mapDims);
    }
  }

  // low pass filter
  for (int r = 0; r < _mapDims*2; r++) {
    for (int c = 0; c < _mapDims*2; c++) {
      _map.at<float>(r,c) = _map.at<float>(r,c)*_alpha + (1-_alpha)*_tempMap.at<float>(r,c);
    }
  }
}

// plot a square on the visualizer with the specified radius (s) and grayscale intensity (v)
void Map2D::_plotPoint(int x, int y, int s, float v) {
  for(int i = -s; i <= s; i++) {
    for(int j = -s; j <= s; j++) {
      _visualMat.at<float>(i+y,j+x) = v;
    }
  }
}

void Map2D::_addToMap(Point3D p, int w, int h, int cent_x, int cent_y) {
  // reproject to 3d
  float x = (p.x-0.5f*w)/p.z;
	float y = (-p.y+0.5f*h)/p.z;
	float z = (-0.8*w)/p.z;

  // calculate the approximate row and column of the point
  int r = cent_y+round(z*_multiplier);
  int c = cent_x+round(x*_multiplier);

  // make sure in bounds
  if(r >= 0 && r < _mapDims*2 && c >= 0 && c < _mapDims*2)
    // increment the value by the y weight / z*10 to give point's further away less weight
    _tempMap.at<float>(r,c) = _tempMap.at<float>(r,c) + (_getYWeight(y)/(-z*_meshInverseScalar));
  else
    std::cout << "Error: OOB" << std::endl;
}

// return the weight of the pixel based on it's y value
float Map2D::_getYWeight(float y) {
  if (y < _yStops[0])
    return _yWeights[0];
  for(int i = 0; i < _yStops.size(); i++) {
    if(y >= _yStops[i])
      return _yWeights[i+1];
  }
  return 0.0f;
}

void Map2D::displayMap() {
  for (int r = 1; r < _mapDims*2-1; r++) {
    for (int c = 1; c < _mapDims*2-1; c++) {
      // plot each point with the given intensity/200
      _plotPoint(c*_scaler, r*_scaler, _scaler/2, _map.at<float>(r,c)/100.f);
    }
  }

  // plot R2
  _plotPoint(Map2DSize, Map2DSize, 5, 1.0f);

  cv::imshow("2D Map", _visualMat);
}
