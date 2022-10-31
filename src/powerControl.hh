#pragma once

#include "senecResultObserver.hh"
#include "senecResultSubject.hh"
#include "senecResultDto.hh"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>
#include <iostream>
#include <chrono>


namespace SDA
{
  class PowerControl
  {
  public:
    PowerControl(boost::asio::io_context& ioContext,
                 unsigned TimerDuration,
                 SenecResultSubject& arResultSubject);
  private:
    void Control();

    unsigned mTimerDuration;
    boost::asio::steady_timer mTimer;
    SenecResultSubject& mrResultSubject;
    SenecResultObserver mSenecResultObserver;
  };
}