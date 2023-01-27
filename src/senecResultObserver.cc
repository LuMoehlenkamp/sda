#include "senecResultObserver.hh"

using namespace SDA;

int SenecResultObserver::mStaticNumber = 0;

SenecResultObserver::SenecResultObserver(SenecResultSubject &subject)
    : mrSubject(subject), mrLogger(my_logger::get()) {
  mrSubject.Attach(this);
  ++SenecResultObserver::mStaticNumber;
  mNumber = SenecResultObserver::mStaticNumber;
  BOOST_LOG_SEV(mrLogger, normal)
      << "Observer_" << mNumber << " attached to Subject";
}

SenecResultObserver::~SenecResultObserver() {
  BOOST_LOG_SEV(mrLogger, normal) << "SenecResultObserver::Ctor called";
}

void SenecResultObserver::Update(const SenecResultDto &arDataFromSubject) {
  mSenecResultDto = arDataFromSubject;
  BOOST_LOG_SEV(mrLogger, normal)
      << "SenecResultObserver::Update received new message from subject";
  PrintInfo(arDataFromSubject);
}

void SenecResultObserver::RemoveMeFromTheList() {
  mrSubject.Detach(this);
  BOOST_LOG_SEV(mrLogger, normal) << "SenecResultObserver::Observer \""
                                  << mNumber << "\" removed from the list";
}

void SenecResultObserver::PrintInfo(const SenecResultDto &arDataFromSubject) {
  BOOST_LOG_SEV(mrLogger, normal)
      << "SenecResultObserver::Observer_" << this->mNumber
      << ": a new message is available";
  time_t tt;
  tt = std::chrono::system_clock::to_time_t(
      arDataFromSubject.mTimeOfMeasurement);
  BOOST_LOG_SEV(mrLogger, normal) << "meas time: " << ctime(&tt);
  BOOST_LOG_SEV(mrLogger, normal)
      << "charging level: " << arDataFromSubject.mChargeLevel
      << "| power bat: " << arDataFromSubject.mPowerBat
      << "| power gen: " << arDataFromSubject.mPowerGen
      << "| power grid: " << arDataFromSubject.mPowerGrid
      << "| power house: " << arDataFromSubject.mPowerHouse;
}

const SenecResultDto &SenecResultObserver::GetLatestMeasurement() const {
  return mSenecResultDto;
}