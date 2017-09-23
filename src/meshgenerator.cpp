#include "meshgenerator.h"

MeshGenerator::MeshGenerator() {
	// the minimum depth value to consider
  _minValue = 400;

	// the resolution of the mesh (1 is the best but takes the longest)
	// also determines pixel's neighboring distance
  _resolution = 4;

	// the threshold between two neighboring pixel's depth gradient to put them on the same mesh
  // usually ~ 4*resolution
  _diffThreshold = 16;

	// the minimum number of faces a mesh must have
	// usually ~ 200/resolution
  _minMeshes = 50;

	// ignore points to the left of this x pos (always 0)
	_clipXLeft = 90;

  std::vector<Mesh> _meshes;
}

MeshGenerator::~MeshGenerator() {}

std::vector<Mesh>* MeshGenerator::getMeshes() {
  return &_meshes;
}

void MeshGenerator::generateMesh(cv::Mat* input) {
	_meshes.clear();
  int w = input->size().width;
  int h = input->size().height;
  
  int next_mesh = 1;

  // 0 means not part of mesh, i>0 means part of mesh i
  _meshIdx = cv::Mat::zeros(input->size(),  CV_32S);

  // 0 means not visited, 1 means visited
  _visited = cv::Mat::zeros(input->size(),  CV_8U);

  // if on a queue, then you have been visited
  std::queue<PointRC> unassigned_mesh_edge_queue;

  // put first point on queue and mark as visited
  PointRC first_point = {.r = 0, .c = _clipXLeft};
  unassigned_mesh_edge_queue.push(first_point);
  _visit(first_point.r, first_point.c);

  while(!unassigned_mesh_edge_queue.empty()) {
    PointRC next_point = unassigned_mesh_edge_queue.front();
    unassigned_mesh_edge_queue.pop();

    // Check if already assigned mesh, and skip this point if it is
    if (_meshIdx.at<int>(next_point.r, next_point.c) != 0) {
      continue;
    }

    short value = input->at<short>(next_point.r, next_point.c);
    if(value > _minValue) {
      // if we can make a new mesh, start one
      _iterateNewMesh(next_mesh++, next_point, input, unassigned_mesh_edge_queue);
    } else {
      // if this point is useless, enqueue its surrounding points
      for (int i = -_resolution; i <= _resolution; i+= _resolution) {   // r modifier
        for (int j = -_resolution; j <= _resolution; j+= _resolution) { // c modifier
          // skip this point
          if (i == 0 && j == 0) continue;

          int next_r = next_point.r + i;
          int next_c = next_point.c + j;

          // check if valid and not visited
          if (_pointInBounds(next_r, next_c, h, w) && _not_visited(next_r, next_c)) {
            // add to unassigned queue
            unassigned_mesh_edge_queue.push({.r = next_r, .c = next_c});

            // we added it to a queue, so it has been visited
            _visit(next_r, next_c);
          }
        }
      }
    }
  }

  // remove the small meshes, TODO update mesh and point mats
  for(int j = 0; j < _meshes.size(); j++) {
    if(_meshes[j].faces.size() < _minMeshes) {
      // zero out _meshIdx
      for (int i = 0; i < _meshes[j].points.size(); i++) {
        Point3D point = _meshes[j].points[i];
        _fillMesh(point.y, point.x, 0);
      }

	    _meshes.erase(_meshes.begin() + j);
      j--;
    }
  }

  _pointIdx = cv::Mat::zeros(input->size(),  CV_8U);
  cv::compare(_meshIdx, 0, _pointIdx, cv::CMP_GT);
}

void MeshGenerator::_iterateNewMesh(
  int id, 
  PointRC first_point,
  cv::Mat *input,
  std::queue<PointRC>& unassigned_mesh_edge_queue
) {
  std::queue<PointRC> current_mesh_edge_queue;

  int w = input->size().width;
  int h = input->size().height;

  // Handle first point
  current_mesh_edge_queue.push(first_point);

  // should have already been visited
  
  // set the mesh index matrix for the new point
  _fillMesh(first_point.r, first_point.c, id);

  // create new mesh and faces vector
  std::vector<TriangleMesh> faces;
  std::vector<Point3D> points;
  _meshes.push_back({ faces, points });

  while(!current_mesh_edge_queue.empty()) {
    PointRC next_point = current_mesh_edge_queue.front();
    current_mesh_edge_queue.pop();

    int r = next_point.r;
    int c = next_point.c;

    // check all 8 surrounding points
    for (int i = -_resolution; i <= _resolution; i+= _resolution) {   // r modifier
      for (int j = -_resolution; j <= _resolution; j+= _resolution) { // c modifier
        // skip 0,0 (this point)
        if (i == 0 && j == 0) continue;

        int next_r = r + i;
        int next_c = c + j;

        short value = input->at<short>(next_r, next_c);

        // make sure valid point
        if (_pointInBounds(next_r, next_c, h, w)) {
          short next_value = input->at<short>(next_r, next_c);
          // check if point not already in a mesh
          if (_meshIdx.at<int>(next_r, next_c) == 0) {          
            // check if we can add the next point to the new mesh (within threshold)
            if (next_value > _minValue && std::abs(next_value - value) < _diffThreshold) {
              // set the mesh and point index matricies for the new point in the mesh
              _fillMesh(next_r, next_c, id);

              // only add to queue if not visited
              if (_not_visited(next_r, next_c)) {
                // add to points vector
                points.push_back({.x = next_c, .y = next_r, .z = next_value});
                // put it on the current mesh edge queue to process
                current_mesh_edge_queue.push({.r = next_r, .c = next_c});
              }
            } else {
              // only add to queue if not visited
              if (_not_visited(next_r, next_c)) {
                // put it on the unassigned mesh edge queue to 
                unassigned_mesh_edge_queue.push({.r = next_r, .c = next_c});
              }
            }

            // the new point was either already added or we just added it to a queue, so it has been visited
            _visit(next_r, next_c);
          }

          // add faces if the bottom right most point in bounds
          if (i == _resolution && j == _resolution) {
            // Only add the 2 faces between (r,c) and (r+res, c+res) if they exist
            if (_meshIdx.at<int>(next_r, next_c) == id) {
              // check below
              if (_meshIdx.at<int>(next_r, next_c - _resolution) == id) {
                faces.push_back({.p1 = next_point, .p2 = {.r = next_r, .c = next_c}, .p3 = {.r = next_r, .c = next_c - _resolution} });
              }

              // check right
              if (_meshIdx.at<int>(next_r - _resolution, next_c) == id) {
                faces.push_back({.p1 = next_point,  .p2 = {.r = next_r - _resolution, .c = next_c}, .p3 = {.r = next_r, .c = next_c} });
              }
            }
          }
        }
      }
    }
  }
}

// in bounds requires point + resolution to be in bounds as well
bool MeshGenerator::_pointInBounds(int r, int c, int h, int w) {
  return r >= 0 && r + _resolution <= h && c >= _clipXLeft && c + _resolution <= w;
}

void MeshGenerator::_fillMesh(int r, int c, int id) {
  for (int i = r; i < r + _resolution; i++) {
    for (int j = c; j < c + _resolution; j++) {
      _meshIdx.at<int>(i, j) = id;
    }
  }
}

void MeshGenerator::_visit(int r, int c) {
  _visited.at<unsigned char>(r, c) = 1;
}

bool MeshGenerator::_not_visited(int r, int c) {
  return _visited.at<unsigned char>(r, c) == 0;
}