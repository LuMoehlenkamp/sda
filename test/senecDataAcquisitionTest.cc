#include "../src/senecDataAcquisition.hh"
#include "../src/senecResultObserver.hh"
#include "../src/senecResultSubject.hh"

#include <boost/test/unit_test.hpp>
#include <boost/timer/timer.hpp>

#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;
using namespace boost::asio;
namespace SDA::TEST {

BOOST_AUTO_TEST_SUITE(SenecDataAcquisitionTest)

BOOST_AUTO_TEST_CASE(data_aquisition_test) {
  BOOST_TEST_MESSAGE("starting unit tests\n");
  io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(ioContext);
  boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23);
  SenecDataAcquisition senec_da(ioContext, sslContext, 10U);
  boost::timer::cpu_timer timer;
  auto &senec_rs = senec_da.GetResultSubject();
  SenecResultObserver observer(senec_rs);
  // sleep_for(1s);

  ioContext.run();
  auto &dto = observer.GetLatestMeasurement();
  std::cout << "power: " << dto.mPowerHouse << '\n';

  // BOOST_REQUIRE(true);
  BOOST_CHECK(dto.mPowerHouse != 0.0f);
  std::cout << timer.format() << '\n';
  BOOST_TEST_MESSAGE("unit testing finished\n");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace SDA::TEST