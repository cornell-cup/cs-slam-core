#include "meshlabio.h"

namespace meshlabio {
  void writePointCloud(std::string fname, StereoCamera& camera) {
    int w = camera.getWidth();
    int h = camera.getHeight();

    cv::Mat Q = reproject_utils::getReprojectMat(w, h, 0.8f);

    cv::Mat points;

    cv::reprojectImageTo3D(*(camera.getDisparity()), points, Q);

    points = points.reshape(1, w*h);
    cv::cvtColor(*(camera.getLeftCamera()->getFrame()), *(camera.getLeftCamera()->getFrame()), cv::COLOR_BGR2RGB);
    cv::Mat colors = camera.getLeftCamera()->getFrame()->reshape(1, w*h);

    std::ofstream myfile;
    myfile.open(fname);
    myfile << plyHeaderPt1 << points.size().height - (leftCuttof*h) << "\n" << plyHeaderPt2 << std::endl;

    for (int i = 0; i < points.size().height; i++) {
      if (i%w >= leftCuttof) {
        myfile << points.at<float>(i, 0) << " " << points.at<float>(i, 1) << " " << points.at<float>(i, 2) << " " << colors.at<int>(i, 0) << " " << colors.at<int>(i, 1) << " " << colors.at<int>(i, 2) << std::endl;
      }
    }

    myfile.close();

    std::cout << "generated point cloud" << std::endl;
  }

  void writeMeshes(std::string fname, cv::Mat* color, std::vector<Mesh>* meshes, int type) {
    std::cout << meshes->size() << std::endl;

    int numMeshes = meshes->size();

    int h = color->size().height;
    int w = color->size().width;

    int numVerts = 0;
    int numFaces = 0;
    
    // compute the total number of verticies and faces
    for (int i = 0; i < numMeshes; i++) {
      numVerts += (*meshes)[i].points.size();
      numFaces += (*meshes)[i].faces.size();
    }

    cv::Mat colors = color->reshape(1, w*h);

    std::ofstream myfile;
    myfile.open(fname);

    // setup header
    myfile << "ply\nformat ascii 1.0\nelement vertex " << numVerts << "\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\nelement face " << numFaces << "\nproperty list uchar int vertex_index\nend_header" << std::endl;

    float projectMat[3];
    cv::Mat pointIdx = cv::Mat::zeros(color->size(),  CV_32S);

    int pointPos = 0;

    for (int r = 0; r < numMeshes; r++) {

      int colorr = (char) (r*333);
      int colorg = (char) (r*651);
      int colorb = (char) (r*17);

      for (int i = 0; i < (*meshes)[r].points.size(); i++) {
        int x = (*meshes)[r].points[i].x;
        int y = (*meshes)[r].points[i].y;
        int z = (*meshes)[r].points[i].z;

        int colIdx = y*w + x;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);

        // no reprojection output
        //myfile << x << " " << y << " " << z << " " << colors.at<int>(colIdx, 2) << " " << colors.at<int>(colIdx, 1) << " " << colors.at<int>(colIdx, 0) << " " << std::endl;
        
        if(type == 0) {
          // true color
          myfile << projectMat[0] << " " << projectMat[1] << " " << projectMat[2] << " " << colors.at<int>(colIdx, 2) << " " << colors.at<int>(colIdx, 1) << " " << colors.at<int>(colIdx, 0) << " " << std::endl;
        } else if(type == 1) {
          // mesh color
          myfile << projectMat[0] << " " << projectMat[1] << " " << projectMat[2] << " " << colorr << " " << colorg << " " << colorb << " " << std::endl;
        }
        // set point idx
        pointIdx.at<int>(y, x) = pointPos++;
      }
    }

    for (int r = 0; r < numMeshes; r++) {
      for (int i = 0; i < (*meshes)[r].faces.size(); i++) {
        int p1 = pointIdx.at<int>((*meshes)[r].faces[i].p1.r, (*meshes)[r].faces[i].p1.c);
        int p2 = pointIdx.at<int>((*meshes)[r].faces[i].p2.r, (*meshes)[r].faces[i].p2.c);
        int p3 = pointIdx.at<int>((*meshes)[r].faces[i].p3.r, (*meshes)[r].faces[i].p3.c);
        myfile << "3 " << p1 << " " << p2 << " " << p3 << " " << std::endl;
      }
    }

    myfile.close();
    std::cout << "generated meshes" << std::endl;
  }
}