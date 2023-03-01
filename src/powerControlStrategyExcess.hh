#pragma once

#include "IPowerControlStrategy.hh"
#include "configManager.hh"
#include "global.hh"
#include "senecResultDto.hh"
#include "wiringPi.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace SDA {

class PowerControlStrategyExcess : public IPowerControlStrategy {
public:
  PowerControlStrategyExcess(); // : mrLogger(my_logger::get()) {}

  unsigned doControl(const std::string &arConfigPath,
                     const SenecResultDto &arSenecResultDto,
                     const bool aTestMode,
                     const bool aGpioInitialized) override;

private:
  bool getParams(const std::string &arConfigPath,
                 boost::optional<bool> &rTestModeOpt,
                 boost::optional<unsigned int> &rExcessPowerThreshholdOpt,
                 boost::optional<unsigned int> &rLoadPowerOpt) const;
  bool checkMeasurementAge(const std::chrono::_V2::system_clock::time_point
                               &rTimeOfMeasurement) const;
  bool checkPreconditions(const bool aTestMode,
                          const bool aGpioInitialized) const;
  float calcExportPower(
      float aPowerGrid,
      const boost::optional<unsigned int> &arThresholdPowerOpt) const;
  float normAndLimit(float aInputValue, float aBaseValue) const;

  my_logger::logger_type &mrLogger;
  unsigned mDutyCycle = 0;
};
} // namespace SDA