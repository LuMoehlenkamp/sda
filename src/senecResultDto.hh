#pragma once

#include <chrono>

namespace SDA {

struct SenecResultDto {
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  float mPowerGrid;   // positive values: consumption
  float mPowerGen;    // negative values: generation
  float mPowerHouse;  // positive values: consumption
  float mPowerBat;    // positive values: charging
  float mChargeLevel; // always positive
};

} // namespace SDA