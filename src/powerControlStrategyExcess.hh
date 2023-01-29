#pragma once

#include "IPowerControlStrategy.hh"
#include "global.hh"
#include "senecResultDto.hh"
#include "wiringPi.h"

#include <iostream>

namespace SDA {

class PowerControlStrategyExcess : public IPowerControlStrategy {
private:
  my_logger::logger_type &mrLogger;
  int mDutyCycle = 0;

public:
  PowerControlStrategyExcess() : mrLogger(my_logger::get()) {}

  void doControl(const SenecResultDto &rSenecResultDto, const bool rTestMode,
                 const bool aGpioInitialized) const override {
    BOOST_LOG_SEV(mrLogger, normal)
        << "PowerControlStrategyExcess: "
        << "control cycle - charging level: " << rSenecResultDto.mChargeLevel
        << " - duty cycle: " << mDutyCycle / 10;

    if (!rTestMode && aGpioInitialized) {
      BOOST_LOG_SEV(mrLogger, normal)
          << "PowerControlStrategyExcess: "
          << "writing duty cycle: " << mDutyCycle / 10 << "\% to gpio";
      pwmWrite(18, mDutyCycle);
    }
  }
};
} // namespace SDA