#pragma once

#include <chrono>
#include <iostream>

#include "global.hh"
#include "senecResultObserver.hh"
#include "senecResultSubject.hh"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>

namespace SDA {
class PowerControl {
public:
  PowerControl(boost::asio::io_context &ioContext, unsigned TimerDuration,
               bool Testmode, SenecResultSubject &arResultSubject);

private:
  bool InitOutput();
  void Control();

  int testval = 0;
  unsigned mTimerDuration;
  bool mTestmode;
  boost::asio::steady_timer mTimer;
  // SenecResultSubject &mrResultSubject;
  SenecResultObserver mSenecResultObserver;
  my_logger::logger_type &mrLogger;
};
} // namespace SDA