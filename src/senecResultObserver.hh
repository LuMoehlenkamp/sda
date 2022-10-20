#pragma once

#include "ISenecResultObserver.hh"
#include "senecResultSubject.hh"

namespace SDA {

class SenecResultObserver : public ISenecResultObserver {
 public:
  SenecResultObserver(SenecResultSubject &subject) 
    : mrSubject_(subject) 
  {
    this->mrSubject_.Attach(this);
    // std::cout << "Hi, I'm the Observer \"" << ++SenecResultObserver::mStaticNumber << "\".\n"; // ToDo: Replace with Logging
    // this->mNumber = SenecResultObserver::mStaticNumber;
  }

  virtual ~SenecResultObserver()
  {
    // std::cout << "Goodbye, I was the Observer \"" << this->mNumber << "\".\n"; // ToDo: Replace with Logging
  }

  void Update(const senecResultDto& arDataFromSubject) override
  {
    PrintInfo(arDataFromSubject);
  }
  
  void RemoveMeFromTheList() {
    mrSubject_.Detach(this);
    // std::cout << "Observer \"" << mNumber << "\" removed from the list.\n"; // ToDo: Replace with Logging
  }

  void PrintInfo(const senecResultDto& arDataFromSubject) {
    // std::cout << "Observer_" << this->mNumber << ": a new message is available\n"; // ToDo: Replace with Logging
    time_t tt;
    tt = std::chrono::system_clock::to_time_t ( arDataFromSubject.mTimeOfMeasurement);
    std::cout << "meas time: " << ctime(&tt)
              << "charging level: " << arDataFromSubject.mChargingLevel << '\n'
              << "power bat: " << arDataFromSubject.mPowerBat << '\n'
              << "power gen: " << arDataFromSubject.mPowerGen << '\n'
              << "power grid: " << arDataFromSubject.mPowerGrid << '\n'
              << "power house: " << arDataFromSubject.mPowerHouse << '\n' << '\n';
  }

 private:
  SenecResultSubject& mrSubject_;
  static int mStaticNumber;
  int mNumber;
};

int SenecResultObserver::mStaticNumber = 0;
}