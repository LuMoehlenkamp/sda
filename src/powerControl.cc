#include "powerControl.hh"
#include "wiringPi.h"

using namespace SDA;

PowerControl::PowerControl(boost::asio::io_context &ioContext,
                           unsigned TimerDuration,
                           SenecResultSubject &arResultSubject)
    : mTimerDuration(TimerDuration),
      mTimer(ioContext, boost::asio::chrono::seconds(1)),
      mrResultSubject(arResultSubject) // ToDo: Maybe not required
      ,
      mSenecResultObserver(arResultSubject), mrLogger(my_logger::get()) {
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}

void PowerControl::Control() {
  mTimer.expires_after(boost::asio::chrono::seconds(mTimerDuration));
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
  auto &dto = mSenecResultObserver.GetLatestMeasurement();

  BOOST_LOG_SEV(mrLogger, normal)
      << "control cycle - charging level: " << dto.mChargingLevel
      << " - duty cycle: " << testval / 10;

  // pwmWrite(18, testval);
  if (testval < 1000)
    testval += 100;
  else
    testval = 0;
}