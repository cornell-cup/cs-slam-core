#ifndef _SLAM_VISION_LOOP
#define _SLAM_VISION_LOOP

#include <string>

#include "stereocamera.h"
#include "meshgenerator.h"
#include "map2d.h"
#include "meshlabio.h"
#include "disparitynamedwindows.h"
#include "featuretracker.h"
#include "transformation.h"

#include <mutex>
#include <chrono>
#include <vector>

class VisionLoop {
  
  public:
    VisionLoop();
    virtual ~VisionLoop();

    void vision_loop();

    MeshGenerator meshGenerator;
    // mutex for the meshes
    std::mutex mesh_lock;

  private:
    int _getCurentTime();
};

#endif


