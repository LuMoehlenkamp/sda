#pragma once

#include "senecResultDto.hh"
#include <string>

namespace SDA {

class IPowerControlStrategy {
public:
  virtual ~IPowerControlStrategy() = default;
  virtual unsigned doControl(const std::string &arConfigPath,
                             const SenecResultDto &rSenecResultDto,
                             const bool aTestMode,
                             const bool aGpioInitialized) = 0;
};
} // namespace SDA
