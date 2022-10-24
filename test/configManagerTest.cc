#include <boost/test/unit_test.hpp>
#include "../src/configManager.hh"
#include <iostream>

BOOST_AUTO_TEST_SUITE(ConfigManagerTest)

BOOST_AUTO_TEST_CASE(GetInstance_ReturnsValidPointerAndExpectedParameters)
{
  std::cout << "starting config-manager-test" << '\n';
  bool expected_testmode(true);
  unsigned expected_senec_update_time = 15;
  unsigned expected_solar_update_time = 3600;

  SDA::ConfigManager* p_config_manager = SDA::ConfigManager::GetInstance("../../src/params.json");
  auto testmode = p_config_manager->GetTestMode();
  auto senec_update_time = p_config_manager->GetSenecUpdateTime();
  auto solar_update_time = p_config_manager->GetSolarUpdateTime();

  BOOST_REQUIRE(p_config_manager != nullptr);
  BOOST_CHECK_EQUAL(expected_testmode, testmode);
  BOOST_CHECK_EQUAL(expected_senec_update_time, senec_update_time);
  BOOST_CHECK_EQUAL(expected_solar_update_time, solar_update_time);
  BOOST_TEST_MESSAGE("configManagerTest finished\n");

}

BOOST_AUTO_TEST_SUITE_END()