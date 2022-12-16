#include "powerControl.hh"
#include "wiringPi.h"

using namespace SDA;

PowerControl::PowerControl(boost::asio::io_context &ioContext,
                           unsigned TimerDuration, bool Testmode,
                           SenecResultSubject &arResultSubject)
    : mTimerDuration(TimerDuration), mTestmode(Testmode),
      mTimer(ioContext, boost::asio::chrono::seconds(1)),
      mSenecResultObserver(arResultSubject), mrLogger(my_logger::get()) {
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}

bool PowerControl::InitOutput() {
  if (!mTestmode) {
    if (wiringPiSetupGpio() == -1) {
      BOOST_LOG_SEV(mrLogger, normal)
          << "wiringpi init unsuccessful... goodbye!";
      return false;
    }

    pinMode(18, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(3840);
    pwmSetRange(1000);
    pwmWrite(18, 1000);
  }
  return true;
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