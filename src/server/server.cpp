#include "server.h"

int Server::_bufferSize = 1<<20;
char *Server::_buffer;
Transformation* Server::_transform;

Server::Server(Transformation* transform) {
  // Create and configure the server
  _server = mg_create_server(NULL);
  mg_set_option(_server, "listening_port", "8080");
  mg_add_uri_handler(_server, "/", _handler);

  _buffer = (char *) malloc(_bufferSize);

  _transform = transform;
}

Server::~Server() {
  // Cleanup, and free server instance
  mg_destroy_server(&_server);
  free(_buffer);
}

void Server::_readBuffer(std::string fname) {
  // cache html
  std::ifstream fileStream(fname);
  char c = 0;
  int size = 0;
  for(int i = 0; i < _bufferSize; i++) {
    c = fileStream.get();
    if(c != -1) {
      _buffer[i] = c;
      size++;
    } else {
      _buffer[i] = 0;
      break;
    }
  }
  std::cout << fname << ":\nSize: " << size << " bytes" << std::endl;
}

int Server::_handler(struct mg_connection *conn) {
  char var1[500], var2[500];

  if (strcmp(conn->uri, "/get_matrix") == 0) {
    std::cout << "POST: " << conn->uri << std::endl;

    // Send reply to the client, showing submitted form values.
    // POST data is in conn->content, data length is in conn->content_len
    mg_send_header(conn, "Content-Type", "text/plain");

    // buffer matrix
    int bufLength = 2048;
    char buffer[bufLength];
    std::stringstream stream;
    stream << *(_transform->getTransform());
    for(int i = 0; i < bufLength; i++) {
      char c = stream.get();
      if(c != -1) {
        buffer[i] = c;
      } else {
        buffer[i] = 0;
        break;
      }
    }

    std::cout << buffer << std::endl;
    std::cout << *(_transform->getTransform()) << std::endl;

    mg_printf_data(conn, buffer);
  } else {
    if (strcmp(conn->uri, "/") == 0) {
      _readBuffer("src/server/res/index.html");
    } else if (strcmp(conn->uri, "/three.min.js") == 0) {
      _readBuffer("src/server/res/three.min.js");
    } else {
      _readBuffer("src/server/res/favicon.ico");
    }
    // Show HTML form.
    std::cout << "Serving: " << conn->uri << std::endl;
    mg_send_data(conn, _buffer, strlen(_buffer));
  }

  return 1;
}

void Server::_pollServer() {
  for (;;) {
    mg_poll_server(_server, 1000);
  }
}

void Server::runServerThread() {
  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(_server, "listening_port"));
  std::thread serverThread(&Server::_pollServer, this);
  serverThread.detach();
}