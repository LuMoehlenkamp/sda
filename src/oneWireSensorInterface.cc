#include "oneWireSensorInterface.hh"

#include <algorithm>
#include <fstream>

using namespace SDA;

// clang-format off
OneWireSensorInterface::OneWireSensorInterface(
    const std::string &arFilePathAndName)
    : mrLogger(my_logger::get())
    , mSensorBasePath(arFilePathAndName) {
  IdentifyTempSensors();
}
// clang-format on

void OneWireSensorInterface::IdentifyTempSensors() {
  if (!IsSensorBasePathValid())
    return;

  try {
    for (auto &&entry : fs::directory_iterator(mSensorBasePath)) {
      std::string dirname{entry.path().filename().c_str()};
      if (dirname.find(mTempGroupIdentifier) != std::string::npos)
        mSensorsVec.emplace_back(entry);
    }
  } catch (fs::filesystem_error &er) {
    return;
  }
}

bool OneWireSensorInterface::IsSensorBasePathValid() {
  bool base_path_exists, is_directory, has_root_directory;
  try {
    base_path_exists = fs::exists(mSensorBasePath);
    is_directory = fs::is_directory(mSensorBasePath);
    has_root_directory = mSensorBasePath.has_root_directory();

    if (base_path_exists && is_directory && has_root_directory)
      return true;
  } catch (fs::filesystem_error &er) {
    return false;
  }
  return false;
}

void OneWireSensorInterface::GetAllTemperatures(
    TemperatureSensorsDto &rTemperatures) {
  auto time_of_measurement = std::chrono::system_clock::now();
  rTemperatures.mTimeOfMeasurement = time_of_measurement;
  for (auto path : mSensorsVec) {
    auto temperature_path = path.append("temperature");
    if (fs::is_regular_file(temperature_path)) {
      std::fstream file;
      file.open(temperature_path.c_str(), std::ios::in);
      if (file.is_open()) {
        std::string temp_string;
        std::getline(file, temp_string);
        auto temperature = std::stoul(temp_string);
        rTemperatures.tank_temperature_opt = temperature;
      }
      file.close();
    }
  }
}

void OneWireSensorInterface::GetReservoirTemperature() {}
void OneWireSensorInterface::GetCabinetTemperature() {}
