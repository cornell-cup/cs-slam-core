#ifndef _SLAM_MESHLAB_IO
#define _SLAM_MESHLAB_IO

#include "stereocamera.h"
#include "meshgenerator.h"
#include "reprojectutils.h"

#include <string>
#include <iostream>
#include <fstream>

namespace meshlabio {
  const std::string plyHeaderPt1 = "ply\n"
  "format ascii 1.0\n"
  "element vertex ";

  const std::string plyHeaderPt2 = "property float x\n"
  "property float y\n"
  "property float z\n"
  "property uchar red\n"
  "property uchar green\n"
  "property uchar blue\n"
  "end_header\n";

  const int leftCuttof = 90;

  void writePointCloud(std::string fname, StereoCamera& camera);

  void writeMeshes(std::string fname, cv::Mat* color, std::vector<Mesh>* meshes, int type);
}

#endif