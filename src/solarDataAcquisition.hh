#pragma once

#include <array>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/variant.hpp>

#include "dataAcquisitionDefines.hh"

namespace SDA
{
  const std::string SOLAR_PATH = "/html/269.htm";
  const std::string SOLAR_HOST = "vorhersage-plz-bereich.solar-wetter.com";
  const std::string SOLAR_IP = "217.160.231.108";

  const std::string GET_REQUEST = GET + BLANK + SOLAR_PATH + BLANK + HTTP_VERSION + CRLF +
                                  HOST + BLANK + SOLAR_HOST + CRLF +
                                  ACCEPT + BLANK + ACCEPT_VALUE + CRLF +
                                  CONNECTION + BLANK + CONNECTION_CLOSE_VALUE + CRLF +
                                  CRLF;

  class SolarDataAcquisition
  {
  public:
    SolarDataAcquisition();
    int operator()();
    void ResolveHandler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::results_type results);
    void ConnectHandler(const boost::system::error_code &ec);
    void ReadHandler(const boost::system::error_code &ec, size_t amountOfBytes);
    void ProcessResponse();

  private:
    const unsigned short mPort = 80;
    boost::asio::ip::tcp::endpoint mEndpoint;
    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mTcpSocket;
    std::array<char, 1024> mDataBuffer = {0};
    std::string mResponse;
    boost::property_tree::ptree mTree;
  };

}