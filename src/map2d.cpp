#include "map2d.h"

const int Map2DSize = 250;

Map2D::Map2D() {
  cv::namedWindow("2D Map");

  _multiplier = _mapDims*0.75f;

  _scaler = Map2DSize/_mapDims;

  _map = cv::Mat::zeros(_mapDims*2,_mapDims*2, CV_32F);
  _visualMat = cv::Mat::zeros(Map2DSize*2,Map2DSize*2, CV_32F);

  _yStops.push_back(-1.f);
  _yWeights.push_back(1.f);
  _yStops.push_back(0.3f);
  _yWeights.push_back(1.f);

  _yWeights.push_back(1.f);

  _weight_per_mesh_point = 0.01f;
}

Map2D::~Map2D() {}

void Map2D::updateMap(std::vector<Mesh>& meshes, int img_width, int img_height) {
  _tempMap = cv::Mat::zeros(_mapDims*2,_mapDims*2, CV_32F);

  for (int m = 0; m < meshes.size(); m++) {
    for (int f = 0; f < meshes[m].faces.size(); f++) {
      _addToMap(meshes[m].points[meshes[m].faces[f].p1], img_width, img_height, _mapDims, _mapDims);
      _addToMap(meshes[m].points[meshes[m].faces[f].p2], img_width, img_height, _mapDims, _mapDims);
      _addToMap(meshes[m].points[meshes[m].faces[f].p3], img_width, img_height, _mapDims, _mapDims);
    }
  }

  for (int r = 0; r < _mapDims*2; r++) {
    for (int c = 0; c < _mapDims*2; c++) {
      _map.at<float>(r,c) = _tempMap.at<float>(r,c);
    }
  }
}

void Map2D::_plotPoint(int x, int y, int s, float v) {
  for(int i = -s; i <= s; i++) {
    for(int j = -s; j <= s; j++) {
      _visualMat.at<float>(i+y,j+x) = v;
    }
  }
}

void Map2D::_addToMap(Point3D p, int w, int h, int cent_x, int cent_y) {
  float x = (p.x-0.5f*w)/p.z;
	float y = (-p.y+0.5f*h)/p.z;
	float z = (-0.8*w)/p.z;

  int r = cent_y+round(z*_multiplier);
  int c = cent_x+round(x*_multiplier);

  if(r >= 0 && r < _mapDims*2 && c >= 0 && c < _mapDims*2)
    _tempMap.at<float>(r,c) = _tempMap.at<float>(r,c) + (_getYWeight(y)*_weight_per_mesh_point/(-z*10));
  else
    std::cout << "Error: OOB" << std::endl;
}

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
      _plotPoint(c*_scaler, r*_scaler, _scaler/2, _map.at<float>(r,c));
    }
  }

  _plotPoint(Map2DSize, Map2DSize, 5, 1.0f);

  cv::imshow("2D Map", _visualMat);
}
