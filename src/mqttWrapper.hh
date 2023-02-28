#pragma once
// #include <mosquitto.h>
#include <mosquittopp.h>

class mqttWrapper : public mosqpp::mosquittopp {
public:
  mqttWrapper(const char *_id, const char *_topic, const char *_host,
              int _port);
  ~mqttWrapper();
  bool send_message(const char *_message);

private:
  const char *host;
  const char *id;
  const char *topic;
  int port;
  int keepalive;

  void on_connect(int rc);
  void on_disconnect(int rc);
  void on_message(const struct mosquitto_message *message);
  void on_publish(int mid);
  void on_subscribe(int mid, int qos_count, const int *granted_qos);
};