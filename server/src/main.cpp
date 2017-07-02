#include <iostream>
#include <thread>
#include <string>
#include <csignal>

#include "opencv2/core/utility.hpp"
#include "easylogging++.h"

#include "WebsocketServer.h"
#include "Worker.h"

INITIALIZE_EASYLOGGINGPP

pl::WebSocketServer websocket_server;
pl::Worker worker;

static void help() {
  std::cout
    << "usage: pl ws_url" << std::endl
    << "       options: -v --verbose -v=N verbosity N is a number from 0-9" << std::endl;
}

const char* keys = "{help h||}"
                   "{port|9000|}"
                   "{version v||}";

void initWebsocketServer(int port) {
  pthread_setname_np(pthread_self(), "WebsocketServer");
  websocket_server.init(port);
}

void initWorker() {
  pthread_setname_np(pthread_self(), "worker");
  worker.run();
}

void terminate(int) {
  std::terminate();
}

int main(int argc, char *argv[]) {
  // to set the verbosity level
  START_EASYLOGGINGPP(argc, argv);

  cv::CommandLineParser parser(argc, argv, keys);

  if (parser.has("help")) {
    help();
    return 0;
  }


  int port = parser.get<int>("port");

  if (!parser.check()) {
    parser.printErrors();
    return 0;
  }

  std::thread websocket_server_thread;

  if (port > 0)
    websocket_server_thread = std::thread(initWebsocketServer, port);

  std::thread worker_thread(initWorker);

  std::signal(SIGTERM, terminate);

  worker_thread.join();
  websocket_server_thread.join();

  return 0;
}
