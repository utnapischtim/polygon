#include "json.hpp"
#include "easylogging++.h"

#include "Worker.h"
#include "WebsocketServer.h"
#include "Generator.h"
#include "Filter.h"
#include "Point.h"
#include "CommonSetting.h"
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

      else if (job == "common")
        resp[sync_id] = pl::getCommonSettings();

      else if (job == "pointList") {
        pl::Generator chosen_generator = pl::createGenerator(opts["chosenGenerator"]);
        pl::CommonSettingList common_setting_list = pl::createCommonSettingList(opts["commonSettings"]);
        auto filters = pl::createFilterList(opts["activatedFilters"]);

        pl::PointList list = pl::generatePointList(chosen_generator, common_setting_list, filters);

        resp[sync_id] = pl::to_json(list);
      }

      else
        resp[sync_id] = {{"error", "wrong job"}};

    } catch (nlohmann::detail::type_error e) {
      VLOG(1) << "Worker::run exception: " << e.what();
    }

    websocket_server.addMessage(to_string(resp));
  }
}
