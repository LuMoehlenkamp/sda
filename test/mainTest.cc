// #define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_senecDataAcquisition

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(first_test_suite)

BOOST_AUTO_TEST_CASE(first_test_case) 
{
  BOOST_TEST_MESSAGE("starting unit tests\n");
  BOOST_REQUIRE(true);
  BOOST_TEST_MESSAGE("unit testing finished\n");
}

BOOST_AUTO_TEST_SUITE_END()