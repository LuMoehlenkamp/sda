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
      mSenecResultObserver(arResultSubject) {
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}

void PowerControl::Control() {
  std::cout << "I'm controlling... \n";
  mTimer.expires_after(boost::asio::chrono::seconds(mTimerDuration));
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
  auto &dto = mSenecResultObserver.GetLatestMeasurement();
  std::cout << "charging level: " << dto.mChargingLevel << '\n';
  std::cout << "duty cycle: " << testval / 10 << '\n';

  pwmWrite(18, testval);
  if (testval < 1000)
    testval += 100;
  else
    testval = 0;
}