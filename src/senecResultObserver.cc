#include "senecResultObserver.hh"

using namespace SDA;

int SenecResultObserver::mStaticNumber = 0;

SenecResultObserver::SenecResultObserver(SenecResultSubject &subject) 
  : mrSubject(subject) 
{
  this->mrSubject.Attach(this);
  // std::cout << "Hi, I'm the Observer \"" << ++SenecResultObserver::mStaticNumber << "\".\n"; // ToDo: Replace with Logging
  // this->mNumber = SenecResultObserver::mStaticNumber;
}

SenecResultObserver::~SenecResultObserver()
{
  // std::cout << "Goodbye, I was the Observer \"" << this->mNumber << "\".\n"; // ToDo: Replace with Logging
}

void
SenecResultObserver::Update(const SenecResultDto& arDataFromSubject)
{
  mSenecResultDto = arDataFromSubject;
  PrintInfo(arDataFromSubject);
}

void
SenecResultObserver::RemoveMeFromTheList()
{
  mrSubject.Detach(this);
  // std::cout << "Observer \"" << mNumber << "\" removed from the list.\n"; // ToDo: Replace with Logging
}

void
SenecResultObserver::PrintInfo(const SenecResultDto& arDataFromSubject)
{
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

const SenecResultDto& 
SenecResultObserver::GetLatestMeasurement() const
{
  return mSenecResultDto;
}