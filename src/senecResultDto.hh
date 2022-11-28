#pragma once

#include <chrono>

struct SenecResultDto {
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  float mPowerGrid;
  float mPowerGen;
  float mPowerHouse;
  float mPowerBat;
  float mChargingLevel;
};