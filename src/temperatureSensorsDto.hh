#pragma once

#include <boost/optional.hpp>
#include <chrono>

namespace SDA {

struct TemperatureSensorsDto {
  std::chrono::_V2::system_clock::time_point mTimeOfMeasurement;
  boost::optional<unsigned long> cabinet_temperature_opt;
  boost::optional<unsigned long> tank_temperature_opt;
};

} // namespace SDA