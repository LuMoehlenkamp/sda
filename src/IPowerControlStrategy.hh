#pragma once

#include "senecResultDto.hh"

namespace SDA {

class IPowerControlStrategy {
public:
  virtual ~IPowerControlStrategy() = default;
  virtual void doControl(const SenecResultDto &rSenecResultDto,
                         const bool aTestMode, const bool aGpioInitialized) = 0;
};
} // namespace SDA