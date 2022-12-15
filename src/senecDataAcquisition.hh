#pragma once

#include "dataAcquisitionDefines.hh"

#include <array>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include "dataAcquisitionDefines.hh"
#include "global.hh"
#include "senecResultSubject.hh"

namespace SDA {
const std::string SENEC_PATH = "/lala.cgi";

const std::string SENEC_IP = "192.168.178.40";
const std::string SENEC_REQUEST =
    "{\"PM1OBJ1\":{}, \"PM1OBJ2\":{}, \"BAT1OBJ1\":{}, \"STATISTIC\":{}, "
    "\"ENERGY\":{}}";

const std::string POST_REQUEST =
    POST + BLANK + SENEC_PATH + BLANK + HTTP_VERSION + CRLF + HOST + BLANK +
    HTTP + SENEC_IP + CRLF + USER_AGENT + BLANK + USER_AGENT_VALUE + CRLF +
    CONTENT_TYPE + BLANK + CONTENT_TYPE_VALUE + CRLF + CONTENT_LENGTH + BLANK +
    std::to_string(SENEC_REQUEST.length()) + CRLF + ACCEPT + BLANK +
    ACCEPT_VALUE + CRLF + CONNECTION + BLANK + CONNECTION_KEEP_ALIVE_VALUE +
    CRLF + CRLF + SENEC_REQUEST;

const std::string P_TOTAL_PCC = "PM1OBJ1.P_TOTAL";
const std::string P_TOTAL_PV = "PM1OBJ2.P_TOTAL";

class SenecDataAcquisition {
public:
  SenecDataAcquisition(boost::asio::io_context &ioContext,
                       unsigned TimerDuration);
  void Aquire();
  void
  ResolveHandler(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::resolver::results_type &endpoints);
  void ConnectHandler(const boost::system::error_code &ec);
  void WriteRequestHandler(const boost::system::error_code &ec);
  void ReadStatusHandler(const boost::system::error_code &ec);
  void ReadHeaderHandler(const boost::system::error_code &ec);
  void ReadContentHandler(const boost::system::error_code &ec);
  void ProcessResponse();
  ConversionResultOpt GetGridPower() const;
  ConversionResultOpt GetGeneratorPower() const;
  SenecResultSubject &GetResultSubject();

private:
  const unsigned short mPort = 80;
  boost::asio::ip::tcp::endpoint mEndpoint;
  boost::asio::io_context &mrIoContext;
  boost::asio::ip::tcp::resolver mResolver;
  boost::asio::ip::tcp::socket mTcpSocket;
  ptree mTree;
  unsigned mTimerDuration;
  boost::asio::steady_timer mTimer;
  boost::asio::streambuf mRequest;
  boost::asio::streambuf mResponse;
  SenecResultSubject mrSubject;
  my_logger::logger_type &mrLogger;
};

} // namespace SDA