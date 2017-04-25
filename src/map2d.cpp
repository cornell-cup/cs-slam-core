#include "map2d.h"

Map2D::Map2D(int w, int h) {
  _cent_x = w;
  _cent_y = h;

  cv::namedWindow("2D Map");

  _weight_per_mesh_point = 0.1;
  _multiplier = w*0.75f;
}

Map2D::~Map2D() {}

void Map2D::updateMap(std::vector<Mesh>& meshes, int img_width, int img_height) {
  _map = cv::Mat::zeros(_cent_x*2, _cent_y*2, CV_32F);
  _plotPoint(_cent_x, _cent_y, 1.0f);

  for (int m = 0; m < meshes.size(); m++) {
    for (int f = 0; f < meshes[m].faces.size(); f++) {
      _addToMap(meshes[m].points[meshes[m].faces[f].p1], img_width, img_height);
      _addToMap(meshes[m].points[meshes[m].faces[f].p2], img_width, img_height);
      _addToMap(meshes[m].points[meshes[m].faces[f].p3], img_width, img_height);
    }
  }
}

void Map2D::_plotPoint(int x, int y, float v) {
  for(int i = -5; i <= 5; i++) {
    for(int j = -5; j <= 5; j++) {
      _map.at<float>(i+y,j+x) = v;
    }
  }
}

void Map2D::_addToMap(Point3D p, int h, int w) {
  float x = (p.x-0.5f*w)/p.z;
	float y = (-p.y+0.5f*h)/p.z;
	float z = (-0.8*w)/p.z;

  int r = _cent_y+round(z*_multiplier);
  int c = _cent_x+round(x*_multiplier);
  _map.at<float>(r,c) = _map.at<float>(r,c)+_weight_per_mesh_point;
}

void Map2D::displayMap() {
  cv::imshow("2D Map", _map);
}
