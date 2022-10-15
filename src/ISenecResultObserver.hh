#pragma once

#include "senecResultDto.hh"

namespace SDA
{
  class ISenecResultObserver
  {
  public:
    virtual ~ISenecResultObserver(){};
    virtual void Update(const senecResultDto& arDataFromSubject) = 0;
  };

}