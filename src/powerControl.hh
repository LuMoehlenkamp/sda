#pragma once

#include <chrono>
#include <iostream>

#include "IPowerControlStrategy.hh"
#include "configManager.hh"
#include "global.hh"
#include "powerControlStrategyDefault.hh"
#include "powerControlStrategyExcess.hh"
#include "senecResultObserver.hh"
#include "senecResultSubject.hh"
#include "temperatureSensorsDto.hh"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>

namespace SDA {

class GpioManager;

class PowerControl {

  static const unsigned DefaultTimerDuration;

public:
  explicit PowerControl(boost::asio::io_context &ioContext,
                        SenecResultSubject &arResultSubject,
                        std::unique_ptr<IPowerControlStrategy> &&arStrategy =
                            std::make_unique<PowerControlStrategyDefault>());

  static void SignalHandler(int signal);

private:
  bool InitGpio();
  unsigned GetTimerDurationFromConfig() const;
  void Control();
  void SetTimer();
  TemperatureSensorsDto GetTempeartures() const;

  int testval = 0;
  unsigned mTimerDuration;
  boost::asio::steady_timer mTimer;
  SDA::GpioManager *mpGpioManager = nullptr;
  SDA::ConfigManager *mpConfigManager = nullptr;
  SenecResultObserver mSenecResultObserver;
  my_logger::logger_type &mrLogger;
  bool mTestmode = true;
  bool mGpioInitialised = false;
  std::unique_ptr<IPowerControlStrategy> mpControlStrategy;
};
} // namespace SDA