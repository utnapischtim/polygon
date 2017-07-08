#include "json.hpp"
#include "easylogging++.h"

#include "Worker.h"
#include "WebsocketServer.h"
#include "Generator.h"
#include "Filter.h"
#include "Point.h"
#include "utilities.h"

extern pl::WebSocketServer websocket_server;

void pl::Worker::addRequest(nlohmann::json request) {
  request_queue.push(request);
}

void pl::Worker::run() {
  while (true) {
    nlohmann::json resq = request_queue.pop();
    nlohmann::json resp;

    try {
      std::string
        sync_id = resq["syncId"],
        job = resq["job"];

      nlohmann::json opts = resq["opts"];

      if (job == "filter")
        resp[sync_id] = pl::getListOfFilters();

      else if (job == "generator")
        resp[sync_id] = pl::getListOfGenerators();

      else if (job == "pointList") {
        pl::Generator chosen_generator = pl::createGenerator(opts["chosenGenerator"]);

        auto [local_filters, global_filters] = pl::createFilterList(opts["activatedFilters"]);

        PointList list = pl::generatePointList(chosen_generator, local_filters);
        list = pl::filter(list, global_filters);

        resp[sync_id] = pl::to_json(list);
      }
    } catch (nlohmann::detail::type_error e) {
      VLOG(1) << e.what();
    }

    websocket_server.addMessage(to_string(resp));
  }
}
