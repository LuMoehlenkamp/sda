#include "../src/solarDataAcquisition.hh"
#include <boost/test/unit_test.hpp>
#include <boost/timer/timer.hpp>

BOOST_AUTO_TEST_SUITE(SolarDataAcquisitionTest)

BOOST_AUTO_TEST_CASE(first_and_only_test_case) {
  BOOST_TEST_MESSAGE("starting unit tests\n");
  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(ioContext);
  SDA::SolarDataAcquisition solar_da(ioContext, 1U);
  boost::timer::cpu_timer timer;

  solar_da.Acquire();

  std::cout << timer.format() << '\n';
  BOOST_REQUIRE(true);
  BOOST_TEST_MESSAGE("unit testing finished\n");
}

BOOST_AUTO_TEST_SUITE_END()