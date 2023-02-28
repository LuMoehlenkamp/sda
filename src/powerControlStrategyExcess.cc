#include "powerControlStrategyExcess.hh"

using namespace SDA;

PowerControlStrategyExcess::PowerControlStrategyExcess()
    : mrLogger(my_logger::get()) {}

void PowerControlStrategyExcess::doControl(
    const SenecResultDto &rSenecResultDto, const bool rTestMode,
    const bool aGpioInitialized) {
  BOOST_LOG_SEV(mrLogger, normal)
      << "PowerControlStrategyExcess: "
      << "control cycle - charging level: " << rSenecResultDto.mChargeLevel
      << " - duty cycle: " << mDutyCycle / 10; // ToDo: refactor

  boost::optional<bool> testmode_opt;
  boost::optional<unsigned int> threshold_power_opt, load_power_opt;

  if (!getParams(testmode_opt, threshold_power_opt, load_power_opt) ||
      !checkMeasurementAge(rSenecResultDto.mTimeOfMeasurement) ||
      !checkPreconditions(testmode_opt.get(), aGpioInitialized))
    mDutyCycle = 0;

  else {
    auto export_power =
        calcExportPower(rSenecResultDto.mPowerGrid, threshold_power_opt);

    auto norm_exp_power_limited =
        normAndLimit(export_power, load_power_opt.get());

    mDutyCycle = static_cast<int>(norm_exp_power_limited);
  }

  BOOST_LOG_SEV(mrLogger, normal)
      << "PowerControlStrategyExcess: "
      << "writing duty cycle: " << mDutyCycle << "\% to gpio";

  pwmWrite(18, mDutyCycle);
}

bool PowerControlStrategyExcess::getParams(
    boost::optional<bool> &rTestModeOpt,
    boost::optional<unsigned int> &rExcessPowerThreshholdOpt,
    boost::optional<unsigned int> &rLoadPowerOpt) const {
  ConfigManager *pConfigManager(
      SDA::ConfigManager::GetInstance(SDA::ConfigManager::CONFIG_PATH));
  if (pConfigManager == nullptr) {
    BOOST_LOG_SEV(mrLogger, normal)
        << "PowerControlStrategyExcess: "
        << "config manager not initialized correctly";
    return false;
  }

  rTestModeOpt = pConfigManager->GetTestMode();
  rExcessPowerThreshholdOpt = pConfigManager->GetExcessPowerThreshold();
  rLoadPowerOpt = pConfigManager->GetLoadPower();

  if (!rTestModeOpt.is_initialized() ||
      !rExcessPowerThreshholdOpt.is_initialized() ||
      !rLoadPowerOpt.is_initialized()) {
    BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                    << "params not initialized correctly";
    return false;
  }
  return true;
}

bool PowerControlStrategyExcess::checkMeasurementAge(
    const std::chrono::_V2::system_clock::time_point &rTimeOfMeasurement)
    const {
  auto time_now = std::chrono::system_clock::now();
  auto age_ns = time_now - rTimeOfMeasurement;
  auto age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(age_ns);
  std::chrono::milliseconds max_age_ms(10000); // ToDo: make this a parameter;
  if (age_ms > max_age_ms) {
    BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                    << "measurements too old";
    return false;
  }
  return true;
}

bool PowerControlStrategyExcess::checkPreconditions(
    const bool aTestMode, const bool aGpioInitialized) const {
  if (aTestMode || !aGpioInitialized) {
    BOOST_LOG_SEV(mrLogger, normal) << "PowerControlStrategyExcess: "
                                    << "preconditions not fullfilled";
    return false;
  }
  return true;
}

float PowerControlStrategyExcess::calcExportPower(
    float aPowerGrid,
    const boost::optional<unsigned int> &arThresholdPowerOpt) const {
  auto export_power(-1.0f * aPowerGrid);
  export_power -= static_cast<float>(arThresholdPowerOpt.get());
  return export_power;
}

float PowerControlStrategyExcess::normAndLimit(float aInputValue,
                                               float aBaseValue) const {
  auto normalized_export_power(1000.0f * aInputValue /
                               static_cast<float>(aInputValue));
  auto norm_exp_power_limited(
      std::max(0.0f, std::min(normalized_export_power, 1000.0f)));
  return norm_exp_power_limited;
}