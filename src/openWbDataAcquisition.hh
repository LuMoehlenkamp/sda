#pragma once

#include "dataAcquisitionDefines.hh"
#include "global.hh"

#include <boost/asio.hpp>
#include <chrono>
#include <functional>
#include <iostream>

#include <mqtt/async_client.h>

// #include <mqtt_client_cpp.hpp>

namespace SDA {

class OpenWbDataAcquisition {
public:
  OpenWbDataAcquisition(boost::asio::io_context &ioContext,
                        unsigned TimerDuration)
      : mrIoContext(ioContext), mTimerDuration(TimerDuration),
        mTimer(ioContext, std::chrono::seconds(INITIAL_TIMER_DURATION)),
        // mpClient(
        //     MQTT_NS::make_async_client(mrIoContext, "192.168.178.20", 1883)),
        mrLogger(my_logger::get()) {
    mTimer.async_wait(boost::bind(&OpenWbDataAcquisition::Acquire, this));
    // auto c = MQTT_NS::make_async_client(mrIoContext, "localhost", 1883);

    // mpClient->set_client_id("cid1");
    // mpClient->set_clean_session(true);

    auto disconnect = [&] {
      // if (++count == 5)
      // mpClient->async_disconnect();
    };
  }

  void Setup() {}

  void Acquire() {
    mTimer.expires_after(std::chrono::seconds(mTimerDuration));
    mTimer.async_wait(boost::bind(&OpenWbDataAcquisition::Acquire, this));

    // mpClient->set_close_handler(
    //     std::bind(&OpenWbDataAcquisition::CloseHandler, this));
    // mpClient->set_connack_handler
  }

  void CloseHandler() { std::cout << "close handler called" << '\n'; }

private:
  boost::asio::io_context &mrIoContext;
  boost::asio::steady_timer mTimer;
  unsigned mTimerDuration;
  // std::shared_ptr<mqtt::callable_overlay<mqtt::async_client<
  //     mqtt::tcp_endpoint<boost::asio::ip::tcp::socket, mqtt::strand>>>>
  //     mpClient;
  my_logger::logger_type &mrLogger;
};

} // namespace SDA