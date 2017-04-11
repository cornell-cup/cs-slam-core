#include "profiler.h"

Profiler::Profiler() {}

Profiler::~Profiler() {}

int Profiler::_currentTime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void Profiler::start() {
  _startTime = _currentTime();
  _prevTime = _currentTime();
}

void Profiler::split() {
  int curTime = _currentTime();

  std::cout << "\r" << ((curTime-_prevTime)/1000000);
  _prevTime = curTime;
}

void Profiler::stop() {
  int curTime = _currentTime();

  std::cout << "\r" << ((curTime - _startTime) / 1000000) << std::endl;
}