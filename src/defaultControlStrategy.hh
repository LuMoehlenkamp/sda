#pragma once

#include "IControlStrategy.hh"
#include "global.hh"
#include "senecResultDto.hh"
#include "wiringPi.h"

#include <iostream>

namespace SDA {

class DefaultControlStrategy : public IControlStrategy {
private:
  my_logger::logger_type &mrLogger;
  int mDutyCycle = 500;

public:
  DefaultControlStrategy() : mrLogger(my_logger::get()) {}

  void doControl(const SenecResultDto &rSenecResultDto, const bool rTestMode,
                 const bool aGpioInitialized) const override {
    std::cout << "DefaultControlStrategy" << '\n';
    BOOST_LOG_SEV(mrLogger, normal)
        << "DefaultControlStrategy: "
        << "control cycle - charging level: " << rSenecResultDto.mChargeLevel
        << " - duty cycle: " << mDutyCycle / 10;

    if (!rTestMode && aGpioInitialized) {
      BOOST_LOG_SEV(mrLogger, normal)
          << "DefaultControlStrategy: "
          << "writing duty cycle: " << mDutyCycle / 10 << "\% to gpio";
      pwmWrite(18, mDutyCycle);
    }
  }
};

} // namespace SDA