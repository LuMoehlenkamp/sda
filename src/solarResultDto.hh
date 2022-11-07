#pragma once

#include <chrono>

namespace SDA {

struct SolarResultDto {
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  float mTheoreticalPowerForcast;
  float mActualPowerForcast_min;
  float mActualPowerForcast_max;
};
} // namespace SDA