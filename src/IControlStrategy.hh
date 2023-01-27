#pragma once

#include "senecResultDto.hh"

namespace SDA {

class IControlStrategy {
public:
  virtual ~IControlStrategy() = default;
  virtual void doControl(const SenecResultDto &rSenecResultDto,
                         const bool aTestMode,
                         const bool aGpioInitialized) const = 0;
};
} // namespace SDA
