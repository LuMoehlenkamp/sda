#include "powerControl.hh"
#include "gpioManager.hh"
#include "wiringPi.h"

using namespace SDA;

// clang-format off
PowerControl::PowerControl(boost::asio::io_context &ioContext,
                           unsigned TimerDuration,
                           SenecResultSubject &arResultSubject)
    : mTimerDuration(TimerDuration)
    , mTimer(ioContext, boost::asio::chrono::seconds(1))
    , mpConfigManager( SDA::ConfigManager::GetInstance(SDA::ConfigManager::CONFIG_PATH))
    , mSenecResultObserver(arResultSubject)
    , mrLogger(my_logger::get())
    , mTestmode(true)
    , mGpioInitialised(false)
{
  mpGpioManager = GpioManager::GetInstance();
  if(mpGpioManager != nullptr)
  {
    mTestmode = mpGpioManager->GetTestmodeFromConfig();
    mGpioInitialised = mpGpioManager->InitOutput();
  }
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}
// clang-format on

unsigned PowerControl::GetTimerDurationFromConfig() { return 0; }

void PowerControl::Control() {
  mTimer.expires_after(boost::asio::chrono::seconds(mTimerDuration));
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
  auto &dto = mSenecResultObserver.GetLatestMeasurement();

  BOOST_LOG_SEV(mrLogger, normal)
      << "control cycle - charging level: " << dto.mChargingLevel
      << " - duty cycle: " << testval / 10;
  if (!mTestmode)
    pwmWrite(18, testval);
  if (testval < 1000)
    testval += 100;
  else
    testval = 0;
}