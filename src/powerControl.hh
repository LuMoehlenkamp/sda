#pragma once

#include <chrono>
#include <iostream>

#include "configManager.hh"
#include "global.hh"
#include "senecResultObserver.hh"
#include "senecResultSubject.hh"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>

namespace SDA {

class GpioManager;

class PowerControl {

  static const unsigned DefaultTimerDuration;

public:
  PowerControl(boost::asio::io_context &ioContext,
               SenecResultSubject &arResultSubject);

  static void SignalHandler(int signal);

private:
  unsigned GetTimerDurationFromConfig();
  void Control();
  void SetTimer();

  int testval = 0;
  unsigned mTimerDuration;
  boost::asio::steady_timer mTimer;
  SDA::GpioManager *mpGpioManager = nullptr;
  SDA::ConfigManager *mpConfigManager = nullptr;
  SenecResultObserver mSenecResultObserver;
  my_logger::logger_type &mrLogger;
  bool mTestmode = true;
  bool mGpioInitialised = false;
};
} // namespace SDA