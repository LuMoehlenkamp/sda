#pragma once

#include "IPowerControlStrategy.hh"
#include "configManager.hh"
#include "global.hh"
#include "senecResultDto.hh"
#include "wiringPi.h"

#include <algorithm>
#include <iostream>

namespace SDA {

class PowerControlStrategyExcess : public IPowerControlStrategy {
private:
  my_logger::logger_type &mrLogger;
  int mDutyCycle = 0;

public:
  PowerControlStrategyExcess() : mrLogger(my_logger::get()) {}

  void doControl(const SenecResultDto &rSenecResultDto, const bool rTestMode,
                 const bool aGpioInitialized) override {
    BOOST_LOG_SEV(mrLogger, normal)
        << "PowerControlStrategyExcess: "
        << "control cycle - charging level: " << rSenecResultDto.mChargeLevel
        << " - duty cycle: " << mDutyCycle / 10; // ToDO: refactor

    ConfigManager *pConfigManager(
        SDA::ConfigManager::GetInstance(SDA::ConfigManager::CONFIG_PATH));
    if (pConfigManager == nullptr) {
      BOOST_LOG_SEV(mrLogger, normal)
          << "PowerControlStrategyExcess: "
          << "config manager not initialized correctly";
      return;
    }
    auto testmode_opt = pConfigManager->GetTestMode();
    auto threshold_power_opt = pConfigManager->GetExcessPowerThreshold();
    auto load_power_opt = pConfigManager->GetLoadPower();
    if (!testmode_opt.is_initialized() ||
        !threshold_power_opt.is_initialized() ||
        !load_power_opt.is_initialized()) {
      BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                      << "params not initialized correctly";
      return;
    }

    auto time_now = std::chrono::system_clock::now();
    auto age_ns = time_now - rSenecResultDto.mTimeOfMeasurement;
    auto age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(age_ns);
    std::chrono::milliseconds max_age_ms(10000); // ToDo: make this a parameter;
    if (age_ms > max_age_ms) {
      BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                      << "measurements too old";
      return;
    }

    if (rTestMode || !aGpioInitialized) {
      BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                      << "preconditions not fullfilled";
      return;
    }

    auto export_power(-1.0f * rSenecResultDto.mPowerGrid);
    export_power -= static_cast<float>(threshold_power_opt.get());

    auto normalized_power(1000.0f * export_power /
                          static_cast<float>(load_power_opt.get()));
    auto norm_power_limited(
        std::max(0.0f, std::min(normalized_power, 1000.0f)));
    mDutyCycle = static_cast<int>(norm_power_limited);
    BOOST_LOG_SEV(mrLogger, normal)
        << "PowerControlStrategyExcess: "
        << "writing duty cycle: " << mDutyCycle << "\% to gpio";

    pwmWrite(18, mDutyCycle);
  }
};
} // namespace SDA