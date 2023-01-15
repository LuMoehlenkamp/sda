#pragma once

#include "global.hh"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace SDA {

class OneWireSensorInterface {
public:
  OneWireSensorInterface(const std::string &arFilePathAndName);
  // ~OneWireSensorInterface();

  void GetAllTemperatures(std::vector<unsigned long> &rTemperatures);
  void GetReservoirTemperature();
  void GetCabinetTemperature();

private:
  my_logger::logger_type &mrLogger;
  void IdentifyTempSensors();
  bool IsSensorBasePathValid();
  std::vector<fs::path> mSensorVec;
  fs::path mSensorBasePath;
  std::string mTempGroupIdentifier{"28-"};
};
} // namespace SDA