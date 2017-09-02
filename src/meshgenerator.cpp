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

  cv::Mat meshIdx = cv::Mat::zeros(h, w,  CV_32S);
  cv::Mat pointIdx = cv::Mat::zeros(h, w,  CV_32S);

  int r,c;

  for(r = 0; r < h; r+=_resolution) {
    for(c = _clipXLeft; c < w; c+=_resolution) {
      short value = input->at<short>(r,c);
      if(value > _minValue) {
        int matchIdx = 0;

        // check 4 surrounding points (0, -1), (-1,-1), (-1,0), (-1,1) for acceptable gradient thresholds
        if(c - _resolution >= 0 && std::abs(input->at<short>(r, c - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r, c - _resolution);
        } else if(c - _resolution >= 0 && r - _resolution >= 0 && std::abs(input->at<short>(r - _resolution, c - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c - _resolution);
        } else if(r - _resolution >= 0 && std::abs(input->at<short>(r - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c);
        } else if(c + _resolution < w && r - _resolution >= 0 && std::abs(input->at<short>(r - _resolution, c + _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c + _resolution);
        }

		// check if we can connect the current mesh with the above and to the right mesh
		if (matchIdx != 0 && c + _resolution < w && r - _resolution >= 0 && std::abs(input->at<short>(r - _resolution, c + _resolution) - value) < _diffThreshold) {
			int newMatchIdx = meshIdx.at<int>(r - _resolution, c + _resolution);

			// we can connect, check if different mesh
			if (newMatchIdx != 0 && matchIdx != newMatchIdx) {
				// connect meshes

				// compute point index offset for the faces
				int pointIdxOffset = _meshes[newMatchIdx - 1].points.size();

				// append the points to the connected mesh
				for (int i = 0; i < _meshes[matchIdx - 1].points.size(); i++) {
					int x = _meshes[matchIdx - 1].points[i].x;
					int y = _meshes[matchIdx - 1].points[i].y;
					int z = _meshes[matchIdx - 1].points[i].z;
					_meshes[newMatchIdx - 1].points.push_back({x, y, z});
					pointIdx.at<int>(y, x) = _meshes[newMatchIdx - 1].points.size() - 1;
					meshIdx.at<int>(y, x) = newMatchIdx;
				}
				_meshes[matchIdx - 1].points.clear();
				//_meshes[newMatchIdx - 1].points.insert(_meshes[newMatchIdx - 1].points.end(), _meshes[matchIdx - 1].points.begin(), _meshes[matchIdx - 1].points.end());

				// readjust the faces and add the to the mesh
				for (int i = 0; i < _meshes[matchIdx - 1].faces.size(); i++) {
					_meshes[newMatchIdx - 1].faces.push_back({_meshes[matchIdx - 1].faces[i].p1+pointIdxOffset, _meshes[matchIdx - 1].faces[i].p2 + pointIdxOffset, _meshes[matchIdx - 1].faces[i].p3 + pointIdxOffset});
				}
				_meshes[matchIdx - 1].faces.clear();

				// TODO figure out why this causes a memory error
				// probably something wrong with meshIdx resetting when the matchIdx != _meshes.size()
				// _meshes.erase(_meshes.begin() + (matchIdx - 1));
				matchIdx = newMatchIdx;
			}
		}

        // new mesh
        if (matchIdx == 0) {
			std::vector<Point3D> points;
			std::vector<TriangleMesh> meshes;
          // add this point to the mesh (x,y,z)
			points.push_back({ c, r, value });
			pointIdx.at<int>(r, c) = points.size() - 1;

		  _meshes.push_back({ points, meshes });
          meshIdx.at<int>(r,c) = _meshes.size();
        }
        // append to mesh
        else {
					// TODO why won't this work
          //Mesh mesh = _meshes[matchIdx-1];
			_meshes[matchIdx - 1].points.push_back({ c, r, value });
		  meshIdx.at<int>(r, c) = matchIdx;
          pointIdx.at<int>(r, c) = _meshes[matchIdx - 1].points.size() - 1;

          int p1Idx = c - _resolution >= 0 ? meshIdx.at<int>(r, c - _resolution) : 0;
          int p2Idx = r - _resolution >= 0 ? meshIdx.at<int>(r - _resolution, c) : 0;
          int p3Idx = (c + _resolution < w && r - _resolution >= 0) ? meshIdx.at<int>(r - _resolution, c + _resolution) : 0;

          // create a face with left and top point
          if(matchIdx == p1Idx && matchIdx == p2Idx) {
			  _meshes[matchIdx - 1].faces.push_back({pointIdx.at<int>(r, c), pointIdx.at<int>(r - _resolution, c), pointIdx.at<int>(r,c - _resolution) });
          }

          // create a face with top and top-left point
          if(matchIdx == p2Idx && matchIdx == p3Idx) {
			  _meshes[matchIdx - 1].faces.push_back({pointIdx.at<int>(r, c), pointIdx.at<int>(r - _resolution, c + _resolution), pointIdx.at<int>(r - _resolution, c) });
          }
        }
      }
    }
  }

  // remove the small meshes
  for(r = 0; r < _meshes.size(); r++) {
    if(_meshes[r].faces.size() < _minMeshes) {
	  _meshes.erase(_meshes.begin() + r);
      r--;
    }
  }
}
