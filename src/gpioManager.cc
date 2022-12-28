#include "gpioManager.hh"

using namespace SDA;

GpioManager *GpioManager::mpGpioManager = nullptr;

GpioManager ::GpioManager()
    : mpConfigManager(ConfigManager::GetInstance(ConfigManager::CONFIG_PATH)) {}

GpioManager *GpioManager::GetInstance() {
  if (mpGpioManager == nullptr)
    mpGpioManager = new GpioManager();
  return mpGpioManager;
}

bool GpioManager::GetTestmodeFromConfig() {
  if (mpConfigManager != nullptr) {
    auto testmode_opt = mpConfigManager->GetTestMode();
    if (testmode_opt)
      return testmode_opt.get();
    return false;
  }
  return false;
}

bool GpioManager::InitOutput() {
  if (!mTestmode) {
    if (wiringPiSetupGpio() == -1) {
      // BOOST_LOG_SEV(mrLogger, normal)
      //     << "wiringpi init unsuccessful... goodbye!";
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