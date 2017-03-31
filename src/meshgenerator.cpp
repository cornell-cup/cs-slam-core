#include "meshgenerator.h"

MeshGenerator::MeshGenerator() {
  _minValue = 10;
  _resolution = 4;
  _diffThreshold = 4;
  _minPoints = 20;
  _minMeshes = 30;

  std::vector<Mesh> _meshes;
}

MeshGenerator::~MeshGenerator() {}

void MeshGenerator::writeToFile(std::string fname, cv::Mat* color) {
	std::cout << _meshes.size() << std::endl;

	int numVerts = 0;
	int numFaces = 0;

	int numMeshes = _meshes.size();

	std::vector<int> vertOffsets;

	for (int i = 0; i < numMeshes; i++) {
		vertOffsets.push_back(numVerts);
		numVerts += _meshes[i].points.size();
		numFaces += _meshes[i].faces.size();
	}

	int h = color->size().height;
	int w = color->size().width;

	cv::Mat colors = color->reshape(1, w*h);

	std::ofstream myfile;
	myfile.open(fname);
	myfile << "ply\nformat ascii 1.0\nelement vertex " << numVerts << "\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\nelement face " << numFaces << "\nproperty list uchar int vertex_index\nend_header" << std::endl;

	for (int r = 0; r < numMeshes; r++) {
		for (int i = 0; i < _meshes[r].points.size(); i++) {
			int x = _meshes[r].points[i].x;
			int y = _meshes[r].points[i].y;
			int colIdx = (h-y)*w + x;

			myfile << x << " " << y << " " << _meshes[r].points[i].z << " " << colors.at<int>(colIdx, 2) << " " << colors.at<int>(colIdx, 1) << " " << colors.at<int>(colIdx, 0) << " " << std::endl;
		}
	}

	for (int r = 0; r < numMeshes; r++) {
		int offset = vertOffsets[r];
		for (int i = 0; i < _meshes[r].faces.size(); i++) {
			myfile << "3 " << _meshes[r].faces[i].p1 + offset << " " << _meshes[r].faces[i].p2 + offset << " " << _meshes[r].faces[i].p3 + offset << " " << std::endl;
		}
	}

	myfile.close();
	std::cout << "generated meshes" << std::endl;
}

void MeshGenerator::generateMesh(cv::Mat* input) {
	_meshes.clear();
  int w = input->size().width;
	int h = input->size().height;

  cv::Mat meshIdx = cv::Mat::zeros(h, w,  CV_32S);
  cv::Mat pointIdx = cv::Mat::zeros(h, w,  CV_32S);

  int r,c;

  for(r = 0; r < h; r+=_resolution) {
    for(c = 0; c < w; c+=_resolution) {
      unsigned char value = input->at<unsigned char>(r,c);
      if(value > _minValue) {
        int matchIdx = 0;

        // check 4 surrounding points (0, -1), (-1,-1), (-1,0), (-1,1)
        if(c - _resolution >= 0 && std::abs(input->at<unsigned char>(r, c - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r, c - _resolution);
        } else if(c - _resolution >= 0 && r - _resolution >= 0 && std::abs(input->at<unsigned char>(r - _resolution, c - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c - _resolution);
        } else if(r - _resolution >= 0 && std::abs(input->at<unsigned char>(r - _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c);
        } else if(c + _resolution < w && r - _resolution >= 0 && std::abs(input->at<unsigned char>(r - _resolution, c + _resolution) - value) < _diffThreshold) {
          matchIdx = meshIdx.at<int>(r - _resolution, c + _resolution);
        }

		// check if we can connect meshes with above and to the right mesh
		if (matchIdx != 0 && c + _resolution < w && r - _resolution >= 0 && std::abs(input->at<unsigned char>(r - _resolution, c + _resolution) - value) < _diffThreshold) {
			int newMatchIdx = meshIdx.at<int>(r - _resolution, c + _resolution);
			
			// we can connect, check if different mesh
			if (newMatchIdx != 0 && matchIdx != newMatchIdx) {
				// connect meshes

				int pointIdxOffset = _meshes[newMatchIdx - 1].points.size();
				
				// append the points to the connected mesh
				for (int i = 0; i < _meshes[matchIdx - 1].points.size(); i++) {
					int x = _meshes[matchIdx - 1].points[i].x;
					int y = _meshes[matchIdx - 1].points[i].y;
					int z = _meshes[matchIdx - 1].points[i].z;
					_meshes[newMatchIdx - 1].points.push_back({x, y, z});
					pointIdx.at<int>(h-y, x) = _meshes[newMatchIdx - 1].points.size() - 1;
					meshIdx.at<int>(h-y, x) = newMatchIdx;
				}
				_meshes[matchIdx - 1].points.clear();
				//_meshes[newMatchIdx - 1].points.insert(_meshes[newMatchIdx - 1].points.end(), _meshes[matchIdx - 1].points.begin(), _meshes[matchIdx - 1].points.end());

				// readjust the faces and add the to the mesh
				for (int i = 0; i < _meshes[matchIdx - 1].faces.size(); i++) {
					_meshes[newMatchIdx - 1].faces.push_back({_meshes[matchIdx - 1].faces[i].p1+pointIdxOffset, _meshes[matchIdx - 1].faces[i].p2 + pointIdxOffset, _meshes[matchIdx - 1].faces[i].p3 + pointIdxOffset});
				}
				_meshes[matchIdx - 1].faces.clear();

				// _meshes.erase(_meshes.begin() + (matchIdx - 1));
				matchIdx = newMatchIdx;
			}
		}

        // new mesh
        if (matchIdx == 0) {
			std::vector<Point3D> points;
			std::vector<TriangleMesh> meshes;
          // add this point to the mesh (x,y,z)
			points.push_back({ c, h-r, ((int)value) * 2 });
			pointIdx.at<int>(r, c) = points.size() - 1;

		  _meshes.push_back({ points, meshes });
          meshIdx.at<int>(r,c) = _meshes.size();
        } 
        // append to mesh
        else {
          //Mesh mesh = _meshes[matchIdx-1];
			_meshes[matchIdx - 1].points.push_back({ c, h-r, ((int)value)*2 });
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

  std::cout << _meshes.size() << " , ";
  // clean up small meshes
  for(r = 0; r < _meshes.size(); r++) {
    if(_meshes[r].points.size() < _minPoints || _meshes[r].faces.size() < _minMeshes) {
	  _meshes.erase(_meshes.begin() + r);
      r--;
    }
  }
  std::cout << _meshes.size() << std::endl;
}