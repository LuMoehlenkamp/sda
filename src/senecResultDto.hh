#pragma once

#include <chrono>

namespace SDA {

struct SenecResultDto {
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  float mPowerGrid; // positive values: consumption
  float mPowerGen;
  float mPowerHouse; // positive values: consumption
  float mPowerBat;
  float mChargingLevel; // always positive
};

} // namespace SDA