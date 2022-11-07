#pragma once

#include "ISenecResultObserver.hh"

namespace SDA {
class ISenecResultSubject {
public:
  virtual ~ISenecResultSubject(){};
  virtual void Attach(ISenecResultObserver *observer) = 0;
  virtual void Detach(ISenecResultObserver *observer) = 0;
  virtual void Notify(const SenecResultDto &arResultDto) = 0;
};
} // namespace SDA