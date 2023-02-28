#include <iostream>
#include <string.h>

#include "mqttWrapper.hh"

mqttWrapper::mqttWrapper(const char *_id, const char *_topic, const char *_host,
                         int _port)
    : mosquittopp(_id, true) {
  mosqpp::lib_init();    // Mandatory initialization for mosquitto library
  this->keepalive = 120; // Basic configuration setup for myMosq class
  this->id = _id;
  this->port = _port;
  this->host = _host;
  this->topic = _topic;
  // connect_async(host, port, keepalive);    // non blocking connection to
  // broker request
  connect(host, port, keepalive);
  /*
  try
  {
    loop_start();            // Start thread managing connection / publish /
  subscribe
  }
  catch (const std::exception&)
  {
    std::cout << "exception thrown";
  }
  */
}

mqttWrapper::~mqttWrapper() {
  // loop_stop();            // Kill the thread
  mosqpp::lib_cleanup(); // Mosquitto library cleanup
}

bool mqttWrapper::send_message(const char *_message) {
  int ret = publish(NULL, this->topic, strlen(_message), _message, 1, false);
  return (ret == MOSQ_ERR_SUCCESS);
}

void mqttWrapper::on_connect(int rc) {
  if (rc == 0) {
    std::cout << "connected with server" << std::endl;
  } else {
    std::cout << "Impossible to connect with server(" << rc << ")" << std::endl;
  }
}

void mqttWrapper::on_disconnect(int rc) {
  std::cout << "disconnection(" << rc << ")" << std::endl;
}

void mqttWrapper::on_message(const mosquitto_message *message) {
  std::cout << "new message" << '\n';
}

void mqttWrapper::on_publish(int mid) {
  std::cout << "Message (" << mid << ") succeed to be published " << std::endl;
}

void mqttWrapper::on_subscribe(int mid, int qos_count, const int *granted_qos) {
  return;
}