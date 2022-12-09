#pragma once

#include <chrono>
#include <iostream>

#include "senecResultObserver.hh"
#include "senecResultSubject.hh"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>

namespace SDA {
class PowerControl {
public:
  PowerControl(boost::asio::io_context &ioContext, unsigned TimerDuration,
               SenecResultSubject &arResultSubject);

private:
  void Control();
  int testval = 0;
  unsigned mTimerDuration;
  boost::asio::steady_timer mTimer;
  SenecResultSubject &mrResultSubject;
  SenecResultObserver mSenecResultObserver;
};
} // namespace SDA