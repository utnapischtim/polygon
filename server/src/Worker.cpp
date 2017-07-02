#include "json.hpp"
#include "easylogging++.h"

#include "Worker.h"
#include "WebsocketServer.h"
#include "Generator.h"
#include "Filter.h"
#include "utilities.h"

extern pl::WebSocketServer websocket_server;

void pl::Worker::addRequest(nlohmann::json request) {
  request_queue.push(request);
}

void pl::Worker::run() {
  while (true) {
    nlohmann::json
      list,
      request = request_queue.pop();

    std::string
      sync_id = request["syncId"].get<std::string>(),
      job = request["job"].get<std::string>();

    if (job == "filter")
      list = getListOfFilters();

    else if (job == "generator")
      list = getListOfGenerators();

    nlohmann::json resp;
    resp[sync_id] = list;
    websocket_server.addMessage(to_string(resp));
  }
}
