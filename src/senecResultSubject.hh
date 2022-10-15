#pragma once

#include <iostream>
#include <list>
#include <string>

#include "ISenecResultSubject.hh"
#include "ISenecResultObserver.hh"

namespace SDA {

class SenecResultSubject : public ISenecResultSubject
{
public:
  virtual ~SenecResultSubject()
  {
    std::cout << "Goodbye, I was the Subject.\n";
  }

  void Attach(ISenecResultObserver *observer) override
  {
    mObserverList.push_back(observer);
  }

  void Detach(ISenecResultObserver *observer) override
  {
    mObserverList.remove(observer);
  }

  void Notify(const senecResultDto& arResultDto) override
  {
    for (auto it : mObserverList)
    {
      it->Update(arResultDto);
    }
  }

  void HowManyObserver()
  {
    std::cout << "There are " << mObserverList.size() << " observers in the list.\n";
  }

 private:
  std::list<ISenecResultObserver *> mObserverList;
};

}