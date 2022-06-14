#include <boost/test/unit_test.hpp>
#include "../src/solarDataAcquisition.hh"

BOOST_AUTO_TEST_SUITE(SolarDataAcquisitionTest)

BOOST_AUTO_TEST_CASE(first_and_only_test_case)
{
  BOOST_TEST_MESSAGE("starting unit tests\n");
  SDA::SolarDataAcquisition sda;
  int result = sda();
  BOOST_REQUIRE(result == 0);
  BOOST_TEST_MESSAGE("unit testing finished\n");
}

BOOST_AUTO_TEST_SUITE_END()