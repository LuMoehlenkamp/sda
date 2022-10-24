#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>

namespace SDA
{
  const bool DEFAULT_TESTMODE = true;
  const unsigned DEFAULT_SENEC_UPDATE_TIME = 10U;
  const unsigned DEFAULT_SOLAR_UPDATE_TIME = 15U;

  class ConfigManager
  {
  protected:
    ConfigManager(const std::string& arFilePathAndName)
      : mConfigLoaded(false)
      , mFilePathAndName(arFilePathAndName)
      , mTestMode(DEFAULT_TESTMODE)
      , mSenecUpdateTime(DEFAULT_SENEC_UPDATE_TIME)
      , mSolarUpdateTime(DEFAULT_SOLAR_UPDATE_TIME)
    {}
  
    static ConfigManager* mpConfigManager;

  public:
    ConfigManager(ConfigManager& arOther) = delete;
    void operator=(const ConfigManager& arOther) = delete;

    static ConfigManager* GetInstance(const std::string arFilePathAndName);
    bool GetTestMode();
    unsigned GetSenecUpdateTime();
    unsigned GetSolarUpdateTime();
    void LoadConfig();
  private:
    bool mConfigLoaded;
    std::string mFilePathAndName;
    bool mTestMode;
    unsigned mSenecUpdateTime;
    unsigned mSolarUpdateTime;

    void EnsureConfigLoaded();
  };

  ConfigManager* ConfigManager::mpConfigManager = nullptr;

  ConfigManager*
  ConfigManager::GetInstance(const std::string arFilePathAndName)
  {
    if (mpConfigManager == nullptr)
      mpConfigManager = new ConfigManager(arFilePathAndName);
    return mpConfigManager;
  }

  bool
  ConfigManager::GetTestMode()
  {
    EnsureConfigLoaded();
    return mTestMode;
  }

  unsigned
  ConfigManager::GetSenecUpdateTime()
  {
    EnsureConfigLoaded();
    return mSenecUpdateTime;
  }

  unsigned
  ConfigManager::GetSolarUpdateTime()
  {
    EnsureConfigLoaded();
    return mSolarUpdateTime;
  }

  void
  ConfigManager::EnsureConfigLoaded()
  {
    if (mConfigLoaded)
      return;

    LoadConfig();
    mConfigLoaded = true;
  }

  void
  ConfigManager::LoadConfig()
  {
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(mFilePathAndName, tree);
    mTestMode = tree.get("testMode", DEFAULT_TESTMODE);
    mSenecUpdateTime = tree.get("senecUpdateTime_sec", DEFAULT_SENEC_UPDATE_TIME);
    mSolarUpdateTime = tree.get("solarUpdateTime_sec", DEFAULT_SOLAR_UPDATE_TIME);
  }
}
