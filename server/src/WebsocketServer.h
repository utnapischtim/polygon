#ifndef SERVERSTD_HPP_
#define SERVERSTD_HPP_

#include <thread>
#include <cstddef>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "ThreadSafeQueue.h"

namespace ws = websocketpp;

namespace pl {

using server = ws::server<ws::config::asio>;

class WebSocketServer {
public:
  WebSocketServer()
    : port_(), hdl_(), server_(),
      msg_queue_(), send_message_thread_() {}

  virtual ~WebSocketServer() {}

  void init(uint16_t port);
  void run();
  void addMessage(std::string message);
  void send_message_loop();

protected:
  virtual void on_open(ws::connection_hdl hdl);
  virtual void on_message(ws::connection_hdl hdl, server::message_ptr msg);
  virtual void on_close(ws::connection_hdl hdl);

protected:
  uint16_t port_;
  ws::connection_hdl hdl_;
  server server_;
  ThreadSafeQueue<std::string> msg_queue_;
  std::thread send_message_thread_;
};

}
#endif
