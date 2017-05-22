#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include "mongoose.h"

static const int bufferSize = 1<<20;
static char buffer[bufferSize];

static void readBuffer(std::string fname) {
  // cache html
  std::ifstream fileStream(fname);
  char c = 0;
  int size = 0;
  for(int i = 0; i < bufferSize; i++) {
    c = fileStream.get();
    if(c != -1) {
      buffer[i] = c;
      size++;
    } else {
      buffer[i] = 0;
      break;
    }
  }
  std::cout << fname << ":\nSize: " << size << " bytes" << std::endl;
}

static int handler(struct mg_connection *conn) {
  char var1[500], var2[500];

  if (strcmp(conn->uri, "/get_matrix") == 0) {
    // Send reply to the client, showing submitted form values.
    // POST data is in conn->content, data length is in conn->content_len
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_printf_data(conn,
                   "[1, 0, 0, -1.5,"
									 " 0, 1, 0, 1,"
									 " 0, 0, 1, 0,"
									 " 0, 0, 0, 1 ]",
                   conn->content_len, conn->content,
                   conn->content_len, var1, var2);
  } else {
    if (strcmp(conn->uri, "/") == 0) {
      readBuffer("res/index.html");
    } else if (strcmp(conn->uri, "/three.min.js") == 0) {
      readBuffer("res/three.min.js");
    } else {
      readBuffer("res/favicon.ico");
    }
    // Show HTML form.
    std::cout << "Serving: " << conn->uri << std::endl;
    mg_send_data(conn, buffer, strlen(buffer));
  }

  return 1;
}

int main(void) {
  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL);
  mg_set_option(server, "listening_port", "8080");
  mg_add_uri_handler(server, "/", handler);

  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  return 0;
}
