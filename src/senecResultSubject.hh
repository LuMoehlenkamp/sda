#pragma once

#include <iostream>
#include <list>
#include <string>

#include "ISenecResultObserver.hh"
#include "ISenecResultSubject.hh"

namespace SDA {

class SenecResultSubject : public ISenecResultSubject {
public:
  virtual ~SenecResultSubject() {}

  void Attach(ISenecResultObserver *observer) override {
    mObserverList.push_back(observer);
  }

  void Detach(ISenecResultObserver *observer) override {
    mObserverList.remove(observer);
  }

  void Notify(const SenecResultDto &arResultDto) override {
    for (auto it : mObserverList) {
      it->Update(arResultDto);
    }
  }

  void HowManyObserver() {}

private:
  std::list<ISenecResultObserver *> mObserverList;
};

} // namespace SDA