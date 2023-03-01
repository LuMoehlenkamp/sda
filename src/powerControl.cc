#include "powerControl.hh"
#include "gpioManager.hh"
#include "oneWireSensorInterface.hh"
#include "wiringPi.h"

using namespace SDA;

const unsigned PowerControl::DefaultTimerDuration = 10;

// clang-format off
PowerControl::PowerControl(boost::asio::io_context &ioContext,
                           SenecResultSubject &arResultSubject,
                           std::unique_ptr<IPowerControlStrategy> && arStrategy)
    : mTimer(ioContext, boost::asio::chrono::seconds(1))
    , mpConfigManager( SDA::ConfigManager::GetInstance(SDA::ConfigManager::CONFIG_PATH))
    , mSenecResultObserver(arResultSubject)
    , mrLogger(my_logger::get())
    , mpControlStrategy(std::move(arStrategy))
{
  mTimerDuration = GetTimerDurationFromConfig();
  InitGpio();
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}
// clang-format on

void PowerControl::SignalHandler(int signal) {
  gSignalStatus = signal;
  BOOST_LOG_SEV(my_logger::get(), critical)
      << "termination signal received ... goodbye";
  std::cout << '\n' << "received " << gSignalStatus << '\n';
  GpioManager::ResetOutput();
  exit(1);
}

unsigned PowerControl::GetTimerDurationFromConfig() const {
  auto time_opt{mpConfigManager->GetPowerControlCycleTime()};
  return time_opt.get_value_or(DefaultTimerDuration);
}

bool PowerControl::InitGpio() {
  mpGpioManager = GpioManager::GetInstance();
  if (mpGpioManager != nullptr) {
    mTestmode = mpGpioManager->GetTestmodeFromConfig();
    BOOST_LOG_SEV(mrLogger, normal) << "TestMode is: " << mTestmode;
    mGpioInitialised = mpGpioManager->InitOutput();
    BOOST_LOG_SEV(mrLogger, normal)
        << "GPIO initialisation status is: " << mGpioInitialised;
  }
  return mGpioInitialised;
}

void PowerControl::Control() {
  SetTimer();

  auto &dto = mSenecResultObserver.GetLatestMeasurement();

  // ToDo: Select control strategy
  mpControlStrategy->doControl(ConfigManager::CONFIG_PATH, dto, mTestmode,
                               mGpioInitialised);
}

void PowerControl::SetTimer() {
  mTimer.expires_after(boost::asio::chrono::seconds(mTimerDuration));
  mTimer.async_wait(boost::bind(&PowerControl::Control, this));
}

TemperatureSensorsDto GetTempeartures() {
  OneWireSensorInterface sensor_interface("/sys/bus/w1/devices");
  // ToDo: add logging
  TemperatureSensorsDto temperatures;
  sensor_interface.GetAllTemperatures(temperatures);
  return temperatures;
}