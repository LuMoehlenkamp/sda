#include "../src/configManager.hh"
#include <boost/property_tree/exceptions.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

namespace SDA {
namespace TEST {

const char *VALID_CONFIG_PATH = "../../src/params.json";
const char *INVALID_CONFIG_PATH = "../../src/param.json";

struct ConfigManagerTestValidFixture {
  ConfigManagerTestValidFixture() {
    mpConfigManager = SDA::ConfigManager::GetInstance(VALID_CONFIG_PATH);
  }

  ~ConfigManagerTestValidFixture() { mpConfigManager->Reset(); }

  SDA::ConfigManager *mpConfigManager;
};

struct ConfigManagerTestInvalidFixture {
  ConfigManagerTestInvalidFixture() {
    mpConfigManager = SDA::ConfigManager::GetInstance(INVALID_CONFIG_PATH);
  }

  ~ConfigManagerTestInvalidFixture() { mpConfigManager->Reset(); }

  SDA::ConfigManager *mpConfigManager;
};

BOOST_FIXTURE_TEST_SUITE(ConfigManagerTest_Valid, ConfigManagerTestValidFixture)

BOOST_AUTO_TEST_CASE(
    GetInstance_ValidConfigPath_ReturnsValidPointerAndExpectedParameters) {
  std::cout << "starting config-manager-test" << '\n';
  bool expected_testmode(true);
  unsigned expected_senec_update_time = 10;
  unsigned expected_solar_update_time = 3600;
  unsigned expected_control_cycle_time = 3;
  unsigned expected_excess_power_threshold = 500;
  unsigned expected_load_power = 2500;

  auto testmode = mpConfigManager->GetTestMode();
  auto senec_update_time = mpConfigManager->GetSenecUpdateTime();
  auto solar_update_time = mpConfigManager->GetSolarUpdateTime();
  auto control_cycle_time = mpConfigManager->GetPowerControlCycleTime();
  auto excess_power_threshold = mpConfigManager->GetExcessPowerThreshold();
  auto load_power = mpConfigManager->GetLoadPower();

  BOOST_REQUIRE(mpConfigManager != nullptr);
  BOOST_CHECK_EQUAL(expected_testmode, testmode.get());
  BOOST_CHECK_EQUAL(expected_senec_update_time, senec_update_time.get());
  BOOST_CHECK_EQUAL(expected_solar_update_time, solar_update_time.get());
  BOOST_CHECK_EQUAL(expected_control_cycle_time, control_cycle_time.get());
  BOOST_CHECK_EQUAL(expected_excess_power_threshold,
                    excess_power_threshold.get());
  BOOST_CHECK_EQUAL(expected_load_power, load_power.get());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ConfigManagerTest_Invalid,
                         ConfigManagerTestInvalidFixture)

BOOST_AUTO_TEST_CASE(GetInstance_InvalidPath_DoesNotThrow) {
  BOOST_REQUIRE(mpConfigManager != nullptr);

  boost::optional<bool> testmode;
  BOOST_REQUIRE_NO_THROW(testmode = mpConfigManager->GetTestMode());
  BOOST_CHECK(!testmode.is_initialized());

  boost::optional<unsigned> senec_update_time;
  BOOST_REQUIRE_NO_THROW(senec_update_time =
                             mpConfigManager->GetSenecUpdateTime());
  BOOST_CHECK(!senec_update_time.is_initialized());

  boost::optional<unsigned> solar_update_time;
  BOOST_REQUIRE_NO_THROW(solar_update_time =
                             mpConfigManager->GetSolarUpdateTime());
  BOOST_CHECK(!solar_update_time.is_initialized());

  boost::optional<unsigned> control_cycle_time;
  BOOST_REQUIRE_NO_THROW(control_cycle_time =
                             mpConfigManager->GetPowerControlCycleTime());
  BOOST_CHECK(!control_cycle_time.is_initialized());

  boost::optional<unsigned> excess_power_threshold;
  BOOST_REQUIRE_NO_THROW(excess_power_threshold =
                             mpConfigManager->GetExcessPowerThreshold());
  BOOST_CHECK(!excess_power_threshold.is_initialized());

  boost::optional<unsigned> load_power;
  BOOST_REQUIRE_NO_THROW(load_power = mpConfigManager->GetLoadPower());
  BOOST_CHECK(!load_power.is_initialized());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace TEST
} // namespace SDA