#pragma once

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
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
    boost::optional<bool> GetTestMode();
    boost::optional<unsigned> GetSenecUpdateTime();
    boost::optional<unsigned> GetSolarUpdateTime();
    bool LoadConfig();
    void Reset();
    
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

  boost::optional<bool>
  ConfigManager::GetTestMode()
  {
    EnsureConfigLoaded();
    if (mConfigLoaded)
      return mTestMode;
    return boost::none;
  }

  boost::optional<unsigned>
  ConfigManager::GetSenecUpdateTime()
  {
    EnsureConfigLoaded();
    if (mConfigLoaded)
      return mSenecUpdateTime;
    return boost::none;
  }

  boost::optional<unsigned>
  ConfigManager::GetSolarUpdateTime()
  {
    EnsureConfigLoaded();
    if (mConfigLoaded)
      return mSolarUpdateTime;
    return boost::none;
  }

  void
  ConfigManager::EnsureConfigLoaded()
  {
    if (mConfigLoaded)
      return;

    mConfigLoaded = LoadConfig();
  }

  bool
  ConfigManager::LoadConfig()
  {
    try
    {
      boost::property_tree::ptree tree;
      boost::property_tree::read_json(mFilePathAndName, tree);
      mTestMode = tree.get("testMode", DEFAULT_TESTMODE);
      mSenecUpdateTime = tree.get("senecUpdateTime_sec", DEFAULT_SENEC_UPDATE_TIME);
      mSolarUpdateTime = tree.get("solarUpdateTime_sec", DEFAULT_SOLAR_UPDATE_TIME);
    }
    catch(const boost::property_tree::json_parser_error& pt_e)
    {
      std::cerr << pt_e.what() << '\n';
      return false;
    }
    catch(const boost::property_tree::ptree_bad_path& pt_bp_e)
    {
      std::cerr << pt_bp_e.what() << '\n';
      return false;
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
      return false;
    }
    catch (...)
    {
      return false;
    }
    return true;
  }

  void
  ConfigManager::Reset()
  {
    mConfigLoaded = false;
    mFilePathAndName = "";
    mTestMode = DEFAULT_TESTMODE;
    mSenecUpdateTime = DEFAULT_SENEC_UPDATE_TIME;
    mSolarUpdateTime = DEFAULT_SOLAR_UPDATE_TIME;
    mpConfigManager = nullptr;
  }
}
