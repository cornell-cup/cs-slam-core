#ifndef _SLAM_PROFILER
#define _SLAM_PROFILER

#include <chrono>
#include <iostream>

class Profiler {
public:
  Profiler();
  virtual ~Profiler();
  void start();
  void split();
  void stop();

private:
  int _currentTime();
  int _startTime;
  int _prevTime;

};

#endif