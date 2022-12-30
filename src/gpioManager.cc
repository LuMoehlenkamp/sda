#include "gpioManager.hh"

using namespace SDA;

GpioManager *GpioManager::mpGpioManager = nullptr;
bool GpioManager::mTestmode = true;

GpioManager::GpioManager()
    : mpConfigManager(ConfigManager::GetInstance(ConfigManager::CONFIG_PATH)) {}

GpioManager *GpioManager::GetInstance() {
  if (mpGpioManager == nullptr)
    mpGpioManager = new GpioManager();
  return mpGpioManager;
}

bool GpioManager::GetTestmodeFromConfig() {
  if (mpConfigManager != nullptr) {
    auto testmode_opt = mpConfigManager->GetTestMode();
    if (testmode_opt) {
      mTestmode = testmode_opt.get();
      return mTestmode;
    }
    return false;
  }
  return false;
}

bool GpioManager::InitOutput() {
  if (!mTestmode) {
    if (wiringPiSetupGpio() == -1) {
      BOOST_LOG_SEV(my_logger::get(), normal) << "wiringpi init unsuccessful";
      return false;
    }

    pinMode(18, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(3840);
    pwmSetRange(1000);
    pwmWrite(18, 1000);
    BOOST_LOG_SEV(my_logger::get(), normal) << "wiringpi init successful";
    return true;
  }
  BOOST_LOG_SEV(my_logger::get(), normal)
      << "running in testmode- no wiringpi init attempted";
  return false;
}

bool GpioManager::ResetOutput() {
  if (!mTestmode) {
    pinMode(18, INPUT);
    BOOST_LOG_SEV(my_logger::get(), critical) << "resetting gpio pin 18";
    return true;
  }
  return false;
}