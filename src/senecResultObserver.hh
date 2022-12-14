#pragma once

#include "ISenecResultObserver.hh"
#include "global.hh"
#include "senecResultSubject.hh"

namespace SDA {

class SenecResultObserver : public ISenecResultObserver {
public:
  SenecResultObserver(SenecResultSubject &subject);
  virtual ~SenecResultObserver();

  void Update(const SenecResultDto &arDataFromSubject) override;
  void RemoveMeFromTheList();
  void PrintInfo(const SenecResultDto &arDataFromSubject);
  const SenecResultDto &GetLatestMeasurement() const;

private:
  SenecResultSubject &mrSubject;
  static int mStaticNumber;
  int mNumber;
  SenecResultDto mSenecResultDto;
  my_logger::logger_type &mrLogger;
};

} // namespace SDA