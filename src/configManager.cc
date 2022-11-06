#include "configManager.hh"

using namespace SDA;

ConfigManager* ConfigManager::mpConfigManager = nullptr;

ConfigManager::ConfigManager(const std::string& arFilePathAndName)
  : mConfigLoaded(false)
  , mFilePathAndName(arFilePathAndName)
  , mTestMode(DEFAULT_TESTMODE)
  , mSenecUpdateTime(DEFAULT_SENEC_UPDATE_TIME)
  , mSolarUpdateTime(DEFAULT_SOLAR_UPDATE_TIME)
  , mPowerControlCycleTime(DEFAULT_POWER_CONTROL_CYCLE_TIME)
{}

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

boost::optional<unsigned> 
ConfigManager::GetPowerControlCycleTime()
{
  EnsureConfigLoaded();
  if (mConfigLoaded)
    return mPowerControlCycleTime;
  return boost::none;
}

bool
ConfigManager::LoadConfig()
{
  try
  {
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(mFilePathAndName, tree);
    mTestMode = tree.get(TEST_MODE_NAME, DEFAULT_TESTMODE);
    mSenecUpdateTime = tree.get(SENEC_UPDATE_TIME_NAME, DEFAULT_SENEC_UPDATE_TIME);
    mSolarUpdateTime = tree.get(SOLAR_UPDATE_TIME_NAME, DEFAULT_SOLAR_UPDATE_TIME);
    mPowerControlCycleTime = tree.get(POWER_CONTROL_CYCLE_NAME, DEFAULT_POWER_CONTROL_CYCLE_TIME);
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
  mPowerControlCycleTime = DEFAULT_POWER_CONTROL_CYCLE_TIME;
  mpConfigManager = nullptr;
}

void
ConfigManager::EnsureConfigLoaded()
{
  if (mConfigLoaded)
    return;
  mConfigLoaded = LoadConfig();
}