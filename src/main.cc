#include <array>
#include <iostream>
#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "senecDataAcquisition.hh"
#include "solarDataAcquisition.hh"

int main(int argc, char *argv[])
{
  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work =
    boost::asio::make_work_guard(ioContext);

  SDA::SenecDataAcquisition senec_da(ioContext, 15U);
  SDA::SolarDataAcquisition solar_da(ioContext, 15U);

  ioContext.run();
  
  return 0;
}