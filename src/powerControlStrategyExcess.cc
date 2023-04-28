#include "powerControlStrategyExcess.hh"

#include <algorithm>

using namespace SDA;

PowerControlStrategyExcess::PowerControlStrategyExcess()
    : mrLogger(my_logger::get()) {}

unsigned PowerControlStrategyExcess::doControl(
    const std::string &arConfigPath, const SenecResultDto &arSenecResultDto,
    const bool aTestMode, const bool aGpioInitialized) {
  BOOST_LOG_SEV(mrLogger, normal)
      << "PowerControlStrategyExcess: "
      << "control cycle - charging level: " << arSenecResultDto.mChargeLevel
      << " - duty cycle: " << mDutyCycle / 10; // ToDo: refactor

  boost::optional<bool> testmode_opt;
  boost::optional<unsigned int> threshold_power_opt, load_power_opt;

  if (!getParams(arConfigPath, testmode_opt, threshold_power_opt,
                 load_power_opt) ||
      !checkMeasurementAge(arSenecResultDto.mTimeOfMeasurement))
    mDutyCycle = 0;

  else {
    auto act_load_power = calcActLoadPower(load_power_opt);
    auto export_power = calcExportPower(arSenecResultDto.mPowerGrid,
                                        threshold_power_opt, act_load_power);
    auto norm_exp_power_limited =
        normAndLimit(export_power, load_power_opt.get());
    mDutyCycle = static_cast<unsigned>(norm_exp_power_limited);
  }

  BOOST_LOG_SEV(mrLogger, normal)
      << "PowerControlStrategyExcess: "
      << "writing duty cycle: " << mDutyCycle << "\% to gpio";

  if (checkPreconditions(aTestMode, aGpioInitialized))
    pwmWrite(18, mDutyCycle);
  return mDutyCycle;
}

bool PowerControlStrategyExcess::getParams(
    const std::string &arConfigPath, boost::optional<bool> &rTestModeOpt,
    boost::optional<unsigned int> &rExcessPowerThreshholdOpt,
    boost::optional<unsigned int> &rLoadPowerOpt) const {
  ConfigManager *pConfigManager(SDA::ConfigManager::GetInstance(arConfigPath));
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
  auto age_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
      time_now - rTimeOfMeasurement);
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
    float aPowerGrid, const boost::optional<unsigned int> &arThresholdPowerOpt,
    float aActPowerLoad) const {
  auto export_power(-1.0f * aPowerGrid);
  export_power -= static_cast<float>(arThresholdPowerOpt.get());
  export_power += aActPowerLoad;
  return export_power;
}

float PowerControlStrategyExcess::calcActLoadPower(
    const boost::optional<unsigned> &arLoadPowerOpt) const {
  float load_power(static_cast<float>(arLoadPowerOpt.get_value_or(0u)));
  float load_factor(static_cast<float>(mDutyCycle) / 1000.0f);
  float act_load_power(load_power * load_factor);
  return act_load_power;
}

float PowerControlStrategyExcess::normAndLimit(float aInputValue,
                                               float aBaseValue) const {
  auto normalized_export_power(1000.0f * aInputValue /
                               static_cast<float>(aBaseValue));
  auto norm_exp_power_limited(
      std::clamp(normalized_export_power, 0.0f, 1000.0f));
  return norm_exp_power_limited;
}