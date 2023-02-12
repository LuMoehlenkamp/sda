#include "../src/powerControlStrategyExcess.hh"
#include <boost/test/unit_test.hpp>

// float mPowerGrid;   // positive values: consumption
// float mPowerGen;    // negative values: generation
// float mPowerHouse;  // positive values: consumption
// float mPowerBat;    // positive values: charging
// float mChargeLevel; // always positive

namespace TEST {

using namespace SDA;

struct PowerControlStrategyExcessTestFixture {
  PowerControlStrategyExcessTestFixture() {}

  void InitSenecResultDto() {
    auto time_of_measurement = std::chrono::system_clock::now();
    mSenecResultDto.mTimeOfMeasurement = time_of_measurement;
    mSenecResultDto.mPowerGrid = -1000.0f;
    mSenecResultDto.mPowerGen = -3000.0f;
    mSenecResultDto.mPowerHouse = -500.0f;
    mSenecResultDto.mPowerBat = 1500.0f;
    mSenecResultDto.mChargeLevel = 50.0f;
  }

  SenecResultDto mSenecResultDto;
  bool mTestMode = false;
  bool mGpioInitialized = true;
  PowerControlStrategyExcess mUut;
};

BOOST_FIXTURE_TEST_SUITE(PowerControlStrategyExcessTest,
                         PowerControlStrategyExcessTestFixture)
// ToDo make this an fixture test suite
//  clang-format off
BOOST_AUTO_TEST_CASE(ctor_ObjectCreatedSuccessfully) {
  BOOST_TEST_MESSAGE("starting control strategy unit tests\n");

  mUut.doControl(mSenecResultDto, mTestMode, mGpioInitialized);

  BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace TEST