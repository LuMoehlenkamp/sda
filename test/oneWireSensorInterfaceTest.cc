#include "../src/oneWireSensorInterface.hh"
#include "../src/temperatureSensorsDto.hh"

#include <boost/test/unit_test.hpp>

namespace SDA {
namespace TEST {
const char *VALID_SENSOR_PATH = "/home/ludger/source/sda/test/data";
const char *INVALID_SENSOR_PATH = "./data/28-072261372670/temperatur";

BOOST_AUTO_TEST_SUITE(OneWireSensorInterfaceTest)

BOOST_AUTO_TEST_CASE(Ctor_InvalidConfigPath) {
  OneWireSensorInterface uut(INVALID_SENSOR_PATH);
  TemperatureSensorsDto temperatures;

  uut.GetAllTemperatures(temperatures);

  BOOST_REQUIRE(!temperatures.tank_temperature_opt);
}

BOOST_AUTO_TEST_CASE(Ctor_ValidConfigPath) {
  OneWireSensorInterface uut(VALID_SENSOR_PATH);
  TemperatureSensorsDto temperatures;

  uut.GetAllTemperatures(temperatures);

  BOOST_CHECK(!temperatures.cabinet_temperature_opt);
  BOOST_CHECK(temperatures.tank_temperature_opt);
  BOOST_CHECK_EQUAL(temperatures.tank_temperature_opt.get(), 18750);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace TEST
} // namespace SDA