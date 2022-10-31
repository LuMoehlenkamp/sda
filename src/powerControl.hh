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
                 SenecResultSubject& arResultSubject)
      : mTimerDuration(TimerDuration)
      , mTimer(ioContext, boost::asio::chrono::seconds(1))
      , mrResultSubject(arResultSubject) // ToDo: Maybe not required
      , mSenecResultObserver(arResultSubject)
    {
      mTimer.async_wait(boost::bind(&PowerControl::Control, this));
    }
  private:
    void Control()
    {
      std::cout << "I'm controlling... \n";
      mTimer.expires_after(boost::asio::chrono::seconds(mTimerDuration));
      mTimer.async_wait(boost::bind(&PowerControl::Control, this));
      auto& dto = mSenecResultObserver.GetLatestMeasurement();
      std::cout << dto.mChargingLevel << '\n';
    }

    unsigned mTimerDuration;
    boost::asio::steady_timer mTimer;
    SenecResultSubject& mrResultSubject;
    SenecResultObserver mSenecResultObserver;

  };
}