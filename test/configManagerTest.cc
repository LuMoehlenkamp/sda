#include <boost/test/unit_test.hpp>
#include <boost/property_tree/exceptions.hpp>
#include "../src/configManager.hh"
#include <iostream>

namespace SDA { namespace TEST {

const char* VALID_CONFIG_PATH = "../../src/params.json";
const char* INVALID_CONFIG_PATH = "../../src/param.json";

struct ConfigManagerTestValidFixture
{
  ConfigManagerTestValidFixture()
  {
    mpConfigManager = SDA::ConfigManager::GetInstance(VALID_CONFIG_PATH);
  }

  SDA::ConfigManager* mpConfigManager;
};

struct ConfigManagerTestInvalidFixture
{
  ConfigManagerTestInvalidFixture()
  {
    mpConfigManager = SDA::ConfigManager::GetInstance(INVALID_CONFIG_PATH);
  }

  SDA::ConfigManager* mpConfigManager;
};

BOOST_FIXTURE_TEST_SUITE(ConfigManagerTest_Valid, ConfigManagerTestValidFixture)


BOOST_AUTO_TEST_CASE(GetInstance_ValidConfigPath_ReturnsValidPointerAndExpectedParameters)
{
  std::cout << "starting config-manager-test" << '\n';
  bool expected_testmode(true);
  unsigned expected_senec_update_time = 15;
  unsigned expected_solar_update_time = 3600;

  auto testmode = mpConfigManager->GetTestMode();
  auto senec_update_time = mpConfigManager->GetSenecUpdateTime();
  auto solar_update_time = mpConfigManager->GetSolarUpdateTime();

  BOOST_REQUIRE(mpConfigManager != nullptr);
  BOOST_CHECK_EQUAL(expected_testmode, testmode.get());
  BOOST_CHECK_EQUAL(expected_senec_update_time, senec_update_time.get());
  BOOST_CHECK_EQUAL(expected_solar_update_time, solar_update_time.get());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ConfigManagerTest_Invalid, ConfigManagerTestInvalidFixture)

BOOST_AUTO_TEST_CASE(GetInstance_InvalidPath_Throws)
{
  BOOST_REQUIRE_NO_THROW(mpConfigManager->GetTestMode());
  BOOST_REQUIRE(mpConfigManager != nullptr);
  auto testmode = mpConfigManager->GetTestMode();
  auto senec_update_time = mpConfigManager->GetSenecUpdateTime();
  auto solar_update_time = mpConfigManager->GetSolarUpdateTime();

  BOOST_CHECK(!testmode.is_initialized());
}

BOOST_AUTO_TEST_SUITE_END()

}}