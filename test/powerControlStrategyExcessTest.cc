#include "../src/powerControlStrategyExcess.hh"
#include "testDefines.hh"

#include <boost/test/unit_test.hpp>

// float mPowerGrid;   // positive values: consumption
// float mPowerGen;    // negative values: generation
// float mPowerHouse;  // positive values: consumption
// float mPowerBat;    // positive values: charging
// float mChargeLevel; // always positive
namespace SDA {
namespace TEST {

struct PowerControlStrategyExcessTestFixture {
  PowerControlStrategyExcessTestFixture() {}

  void InitSenecResultDtoBasicConsumptionScenario() {
    auto time_of_measurement = std::chrono::system_clock::now();
    mSenecResultDto.mTimeOfMeasurement = time_of_measurement;
    mSenecResultDto.mPowerGrid = 300.0f;
    mSenecResultDto.mPowerGen = 0.0f;
    mSenecResultDto.mPowerHouse = 300.0f;
    mSenecResultDto.mPowerBat = 0.0f;
    mSenecResultDto.mChargeLevel = 0.0f;
  }

  void InitSenecResultDtoBasicExportScenario() {
    auto time_of_measurement = std::chrono::system_clock::now();
    mSenecResultDto.mTimeOfMeasurement = time_of_measurement;
    mSenecResultDto.mPowerGrid = -3000.0f;
    mSenecResultDto.mPowerGen = -7000.0f;
    mSenecResultDto.mPowerHouse = 3000.0f;
    mSenecResultDto.mPowerBat = 1000.0f;
    mSenecResultDto.mChargeLevel = 50.0f;
  }

  SenecResultDto mSenecResultDto;
  bool mTestMode = true;
  bool mGpioInitialized = true;
  PowerControlStrategyExcess mUut;
};

BOOST_FIXTURE_TEST_SUITE(PowerControlStrategyExcessTest,
                         PowerControlStrategyExcessTestFixture)
// ToDo make this an fixture test suite
//  clang-format off
BOOST_AUTO_TEST_CASE(ctor_ObjectCreatedSuccessfully) {
  BOOST_TEST_MESSAGE("starting control strategy unit tests\n");
  InitSenecResultDtoBasicExportScenario();
  unsigned expected_val = 1000;

  auto return_val = mUut.doControl(VALID_CONFIG_PATH, mSenecResultDto,
                                   mTestMode, mGpioInitialized);

  BOOST_CHECK_EQUAL(expected_val, return_val);
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace TEST
} // namespace SDA