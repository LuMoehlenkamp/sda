#include <iostream>

#include <boost/variant.hpp>

namespace SDA
{
  class Converter : public boost::static_visitor<>
  {
    public:
    void operator()(uint& outVal) const
    {
      std::cout << outVal << '\n';
    }

    void operator()(float& outVal) const
    {
      std::cout << outVal << '\n';
    }
  };
}