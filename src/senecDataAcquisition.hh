#pragma once

#include <array>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>

#include "dataAcquisitionDefines.hh"

namespace SDA
{
  
  
  const std::string SENEC_PATH = "/lala.cgi";
  
  const std::string SENEC_IP = "192.168.178.40";
  const std::string SENEC_REQUEST = "{\"PM1OBJ1\":{}, \"PM1OBJ2\":{}, \"BAT1OBJ1\":{}, \"STATISTIC\":{}}";

  const std::string POST_REQUEST = POST + BLANK + SENEC_PATH + BLANK + HTTP_VERSION + CRLF +
                                   HOST + BLANK + HTTP + SENEC_IP + CRLF +
                                   USER_AGENT + BLANK + USER_AGENT_VALUE + CRLF +
                                   CONTENT_TYPE + BLANK + CONTENT_TYPE_VALUE + CRLF +
                                   CONTENT_LENGTH + BLANK + std::to_string(SENEC_REQUEST.length()) + CRLF +
                                   ACCEPT + BLANK + ACCEPT_VALUE + CRLF +
                                   CONNECTION + BLANK + CONNECTION_KEEP_ALIVE_VALUE + CRLF +
                                   CRLF +
                                   SENEC_REQUEST;

  const std::string P_TOTAL_PCC = "PM1OBJ1.P_TOTAL";
  const std::string P_TOTAL_PV = "PM1OBJ2.P_TOTAL";

  
  typedef boost::variant<uint, float> ConversionResult;
  typedef boost::optional<ConversionResult> ConversionResultOpt;
  
  typedef boost::optional<float> float_opt;

  class SenecDataAcquisition
  {
  public:
    SenecDataAcquisition();
    int operator()();
    void ResolveHandler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::results_type results);
    void ConnectHandler(const boost::system::error_code &ec);
    void ReadHandler(const boost::system::error_code &ec, size_t amountOfBytes);
    void ProcessResponse();
    ConversionResultOpt GetGridPower() const;

  private:
    const unsigned short mPort = 80;
    boost::asio::ip::tcp::endpoint mEndpoint;
    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mTcpSocket;
    std::array<char, 1024> mDataBuffer = {0};
    std::string mResponse;
    ptree mTree;
  };

}