#ifndef _SLAM_VISUALIZER
#define _SLAM_VISUALIZER

#include <opencv2/core/core.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Visualizer {
public:
  static Visualizer* instance();
  static void initialize(int res);
  
  virtual ~Visualizer();

  void visualizePoints(cv::Mat* mat);

private:
  Visualizer(int res);
  static Visualizer* _instance;

  static void _display();
  static void _mouse(int button, int state, int x, int y);
  static void _mouseMove(int x, int y);
  static void _init();

  int _resolution;

  bool _mouseDown = false;

  float _xrot = 0.0f;
  float _yrot = 0.0f;

  float _xdiff = 0.0f;
  float _ydiff = 0.0f;
};

#endif