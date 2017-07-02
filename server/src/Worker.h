#ifndef WORKER_H_
#define WORKER_H_

#include <string>

#include "json.hpp"

#include "ThreadSafeQueue.h"


namespace pl {

class Worker {
public:
  Worker() : request_queue() {}
  ~Worker() {}

  void addRequest(nlohmann::json request);
  void run();

private:
  ThreadSafeQueue<nlohmann::json> request_queue;
};


}
#endif
