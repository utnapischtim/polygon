#include <thread>

#include "json.hpp"
#include "easylogging++.h"

#include "WebsocketServer.h"
#include "Worker.h"

using ws::lib::placeholders::_1;
using ws::lib::placeholders::_2;
using ws::lib::bind;

extern pl::Worker worker;

void pl::WebSocketServer::init(uint16_t port) {
  VLOG(1) << "WebsocketServer init port: " << port;

  port_ = port;

  server_.clear_access_channels(ws::log::alevel::all);

  if (VLOG_IS_ON(3)) {
    server_.set_access_channels(ws::log::alevel::connect);
    server_.set_access_channels(ws::log::alevel::disconnect);
    server_.set_access_channels(ws::log::alevel::app);
  }

  server_.init_asio();

  // TODO:
  // this could be a security problem in the future
  server_.set_reuse_addr(true);

  server_.set_open_handler(bind(&WebSocketServer::on_open, this, ::_1));
  server_.set_close_handler(bind(&WebSocketServer::on_close, this, ::_1));
  server_.set_message_handler(bind(&WebSocketServer::on_message, this, ::_1, ::_2));


  // this server listen only to one port, therefore the the
  // send_message thread could be used for every connection that is
  // established for this server!
  send_message_thread_ = std::thread(&WebSocketServer::send_message_loop, this);
  pthread_setname_np(send_message_thread_.native_handle(), "websocket-server-msg-loop");

  run();
}

void pl::WebSocketServer::run() {
  server_.listen(port_);
  server_.start_accept();
  server_.run();
}

void pl::WebSocketServer::addMessage(std::string message) {
  msg_queue_.push(message);
}

void pl::WebSocketServer::send_message_loop() {
  ws::lib::error_code ec;

  while (true) {
    std::string msg = msg_queue_.pop();
    std::string log_msg = msg; // necessary because logging msg before or
                               // after server_.send cause a segfault

    server_.send(hdl_, msg, ws::frame::opcode::text, ec);

    VLOG(2) << "Server send_message_loop msg: " << log_msg;
    VLOG(2) << "Server send_message_loop ec.message(): " << ec.message();
  }
}

void pl::WebSocketServer::on_open(ws::connection_hdl hdl) {
  VLOG(2) << "Server on_open";
  hdl_ = hdl;
}

void pl::WebSocketServer::on_message(ws::connection_hdl /* hdl */, server::message_ptr msg) {
  // so wie die switch anweisung koennte das konzept funktionieren
  // jede abgeleitete klasse kann so die on_message dann
  // implementieren und die gewuenschten funktionalitaeten hinzufuegen!

  // switch (key) {
  // case 'irgendwas':
  //   doIrgendwas
  // }

  VLOG(2) << "WebSocketServer on_message: " <<  msg->get_payload();
  nlohmann::json obj = nlohmann::json::parse(msg->get_payload());
  worker.addRequest(obj);
}

void pl::WebSocketServer::on_close(ws::connection_hdl hdl) {
  // hier sollte die verbindung eigentlich auf null gesetzt werden,
  // damit die sicher nicht mehr verwendet werden kann, um
  // sicherzugehen, dass fehler vermieden werden, auch wenn das hier
  // eigentlich nicht passieren duerfte.

  // auzser wenn es einen verbindungsabbruch gibt, dann muesste die
  // verbindung wieder aufgebaut werden, das koennte hier herinnen
  // angestozsen werden?
  VLOG(2) << "on_close: ";
  // server_.stop_listening();
  try {
    server_.close(hdl, 404, "shutdown");
  } catch (websocketpp::lib::error_code ec) {
    VLOG(2) << "Server on_close websocketpp::lib::error_code: " << ec;
  } catch (websocketpp::exception e) {
    VLOG(2) << "Server on_close exception: " << e.m_msg;
  }
}
