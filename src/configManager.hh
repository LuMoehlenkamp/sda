#pragma once

#include <exception>
#include <iostream>
#include <set>
#include <string>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SDA {
class ConfigManager {
private:
  const bool DEFAULT_TESTMODE = true;
  const unsigned DEFAULT_SENEC_UPDATE_TIME = 10U;
  const unsigned DEFAULT_SOLAR_UPDATE_TIME = 15U;
  const unsigned DEFAULT_POWER_CONTROL_CYCLE_TIME = 7U;

  const std::string TEST_MODE_NAME = "testMode";
  const std::string SENEC_UPDATE_TIME_NAME = "senecUpdateTime_sec";
  const std::string SOLAR_UPDATE_TIME_NAME = "solarUpdateTime_sec";
  const std::string POWER_CONTROL_CYCLE_NAME = "powerControlCycle_sec";

protected:
  ConfigManager(const std::string &arFilePathAndName);

  static ConfigManager *mpConfigManager;

public:
  ConfigManager(ConfigManager &arOther) = delete;
  void operator=(const ConfigManager &arOther) = delete;

  static ConfigManager *GetInstance(const std::string arFilePathAndName);
  boost::optional<bool> GetTestMode();
  boost::optional<unsigned> GetSenecUpdateTime();
  boost::optional<unsigned> GetSolarUpdateTime();
  boost::optional<unsigned> GetPowerControlCycleTime();
  bool LoadConfig();
  void Reset();

private:
  bool mConfigLoaded;
  std::string mFilePathAndName;
  bool mTestMode;
  unsigned mSenecUpdateTime;
  unsigned mSolarUpdateTime;
  unsigned mPowerControlCycleTime;

  void EnsureConfigLoaded();
};

} // namespace SDA
