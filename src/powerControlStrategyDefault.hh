#pragma once

#include "IPowerControlStrategy.hh"
#include "global.hh"
#include "senecResultDto.hh"
#include "wiringPi.h"

#include <iostream>

namespace SDA {

class PowerControlStrategyDefault : public IPowerControlStrategy {
private:
  my_logger::logger_type &mrLogger;
  int mDutyCycle = 0;

public:
  PowerControlStrategyDefault() : mrLogger(my_logger::get()) {}

  void doControl(const SenecResultDto &rSenecResultDto, const bool rTestMode,
                 const bool aGpioInitialized) override {
    BOOST_LOG_SEV(mrLogger, normal)
        << "PowerControlStrategyDefault: "
        << "control cycle - charging level: " << rSenecResultDto.mChargeLevel
        << " - duty cycle: " << mDutyCycle / 10;

    if (!rTestMode && aGpioInitialized) {
      BOOST_LOG_SEV(mrLogger, normal)
          << "PowerControlStrategyDefault: "
          << "writing duty cycle: " << mDutyCycle / 10 << "\% to gpio";
      pwmWrite(18, mDutyCycle);
    }
  }
};
} // namespace SDA