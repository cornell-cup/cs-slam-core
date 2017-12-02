#ifndef _SLAM_SERVER
#define _SLAM_SERVER

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include "mongoose.h"

#include "../transformation.h"

#include <opencv2/core/core.hpp>

class Server {

public:
  Server(Transformation* transform);
  virtual ~Server();

  void runServerThread();

private:
  static int _handler(struct mg_connection *conn);

  static void _readBuffer(std::string fname);

  void _pollServer();

  static int _bufferSize;
  static char *_buffer;

  struct mg_server *_server;

  static Transformation* _transform;
};

#endif