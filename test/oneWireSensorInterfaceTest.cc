#include "../src/oneWireSensorInterface.hh"

#include <boost/test/unit_test.hpp>

namespace SDA {
namespace TEST {
const char *VALID_SENSOR_PATH = "./data/28-072261372670/temperature";
const char *INVALID_SENSOR_PATH = "./data/28-072261372670/temperatur";

BOOST_AUTO_TEST_SUITE(OneWireSensorInterfaceTest)

BOOST_AUTO_TEST_CASE(Ctor_ValidConfigPath) {
  OneWireSensorInterface uut(VALID_SENSOR_PATH);
  std::vector<unsigned long> temperatures;

  uut.GetAllTemperatures(temperatures);

  BOOST_REQUIRE(!temperatures.empty());
  BOOST_REQUIRE(temperatures.size() == 1);
  BOOST_REQUIRE(temperatures.at(0) == 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace TEST
} // namespace SDA