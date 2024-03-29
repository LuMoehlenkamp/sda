#pragma once

#include <wiringPi.h>

#include "configManager.hh"
#include "powerControl.hh"

namespace SDA {

class GpioManager {
private:
  static GpioManager *GetInstance();
  bool GetTestmodeFromConfig();
  bool InitOutput();
  static bool ResetOutput();

protected:
  GpioManager();
  ConfigManager *mpConfigManager = nullptr;
  static GpioManager *mpGpioManager;

public:
  GpioManager(GpioManager &arOther) = delete;
  void operator=(const GpioManager &arOther) = delete;

  friend class PowerControl;

private:
  static bool mTestmode;
};

} // namespace SDA
