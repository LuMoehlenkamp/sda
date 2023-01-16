#pragma once

#include "global.hh"
#include "temperatureSensorsDto.hh"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace SDA {

class OneWireSensorInterface {
public:
  OneWireSensorInterface(const std::string &arFilePathAndName);
  // ~OneWireSensorInterface();

  void GetAllTemperatures(TemperatureSensorsDto &rTemperatures);
  void GetReservoirTemperature();
  void GetCabinetTemperature();

private:
  my_logger::logger_type &mrLogger;
  void IdentifyTempSensors();
  bool IsSensorBasePathValid();
  std::vector<fs::path> mSensorsVec;
  std::map<fs::path, unsigned long> mSensorsValueMap;
  fs::path mSensorBasePath;
  std::string mTempGroupIdentifier{"28-"};
};
} // namespace SDA