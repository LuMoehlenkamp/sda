#pragma once

#include <iostream>
#include <list>
#include <string>

#include "ISenecResultObserver.hh"
#include "ISenecResultSubject.hh"

namespace SDA {

class SenecResultSubject : public ISenecResultSubject {
public:
  virtual ~SenecResultSubject() {
    // std::cout << "Goodbye, I was the Subject.\n"; // ToDo: Replace with
    // Logging
  }

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

  void HowManyObserver() {
    // std::cout << "There are " << mObserverList.size() << " observers in the
    // list.\n";
  }

private:
  std::list<ISenecResultObserver *> mObserverList;
};

} // namespace SDA