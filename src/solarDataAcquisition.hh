#pragma once

#include <array>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "dataAcquisitionDefines.hh"

namespace SDA
{
  const std::string SOLAR_PATH = "/html/269.htm";
  const std::string SOLAR_HOST = "vorhersage-plz-bereich.solar-wetter.com";

  const std::string GET_REQUEST = GET + BLANK + SOLAR_PATH + BLANK + HTTP_VERSION + CRLF +
                                  HOST + BLANK + SOLAR_HOST + CRLF +
                                  ACCEPT + BLANK + ACCEPT_VALUE + CRLF +
                                  CONNECTION + BLANK + CONNECTION_CLOSE_VALUE + CRLF +
                                  CRLF;

  class SolarDataAcquisition
  {
  public:
    SolarDataAcquisition(boost::asio::io_context& ioContext, unsigned int TimerDuration);
    void Aquire();
    void ResolveHandler(const boost::system::error_code& ec, 
                        const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void ConnectHandler(const boost::system::error_code& ec);
    void WriteRequestHandler(const boost::system::error_code& ec);
    void ReadStatusHandler(const boost::system::error_code& ec);
    void ReadHeaderHandler(const boost::system::error_code& ec);
    void ReadContentHandler(const boost::system::error_code& ec);
    void ProcessResponse();

  private:
    const unsigned short mPort = 80;
    boost::asio::io_context& mrIoContext;
    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mTcpSocket;
    ptree mTree;
    unsigned int mTimerDuration;
    boost::asio::steady_timer mTimer;
    boost::asio::streambuf mRequest;
    boost::asio::streambuf mResponse;
  };

}