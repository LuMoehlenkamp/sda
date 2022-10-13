#pragma once

#include <chrono>

struct senecResultDto
{
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  float mTheoreticalPowerForcast;
  float mActualPowerForcast_min;
  float mActualPowerForcast_max;
};