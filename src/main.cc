#include <array>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "senecDataAcquisition.hh"
#include "solarDataAcquisition.hh"


using namespace boost::asio;

// namespace SDA
// {
//   void resolveHandler(const boost::system::error_code &ec, ip::tcp::resolver::iterator iter)
//   {
//     if (ec)
//     {
//       std::cout << "Resolve failed with message: " << ec.message() << '\n';
//     }
//     else
//     {
//       ip::tcp::endpoint endpoint(*iter);
//       std::cout << "Resolved address: " << endpoint.address() << ':' << endpoint.port() << '\n' << '\n';
//     }
//     return;
//   }
// }

int main(int argc, char *argv[])
{
  // std::cout << "Hello, cmake world!\n"
  //           << argv[0] << "\n";
  // try
  // {
  //   io_service ioService;

  //   ip::tcp::resolver resolver(ioService);
  //   ip::tcp::resolver::query query("stackoverflow.com", "8080");
  //   resolver.async_resolve(query, SDA::resolveHandler);
  

  //   ioService.run();
  // }
  // catch(boost::system::system_error& e)
  // {
  //   std::cerr << e.what() << '\n';
  //   return e.code().value();
  // }

  SDA::SenecDataAcquisition sda;
  sda();
  
  SDA::SolarDataAcquisition solar_da;
  solar_da();

  return 0;
}