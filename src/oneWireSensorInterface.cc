#include "oneWireSensorInterface.hh"

#include <algorithm>

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
        mSensorVec.emplace_back(entry);
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
    std::vector<unsigned long> &rTemperatures) {
  rTemperatures.emplace_back(mSensorVec.size());
}
void OneWireSensorInterface::GetReservoirTemperature() {}
void OneWireSensorInterface::GetCabinetTemperature() {}
