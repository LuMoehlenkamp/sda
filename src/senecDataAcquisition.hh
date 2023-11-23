#pragma once

#include "dataAcquisitionDefines.hh"
#include "global.hh"
#include "senecResultSubject.hh"

#include <array>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SDA {

// clang-format off
// POST /lala.cgi HTTP/1.1
// Host: 192.168.178.40
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0
// Accept: application/json, text/javascript, */*; q=0.01
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br
// Content-Type: application/x-www-form-urlencoded; charset=UTF-8
// X-Requested-With: XMLHttpRequest
// Content-Length: 550
// Origin: https://192.168.178.40
// Connection: keep-alive
// Referer: https://192.168.178.40/
// Sec-Fetch-Dest: empty
// Sec-Fetch-Mode: cors
// Sec-Fetch-Site: same-origin

const std::string POST_REQUEST =
    POST + BLANK + SENEC_PATH + BLANK + HTTP_VERSION + CRLF +
    HOST + BLANK + SENEC_IP + CRLF +
    USER_AGENT + BLANK + USER_AGENT_VALUE + CRLF +
    ACCEPT + BLANK + ACCEPT_VALUE + CRLF +
    ACCEPT_LANGUAGE + BLANK + ACCEPT_LANGUAGE_VALUE + CRLF +
    ACCEPT_ENCODING + BLANK + ACCEPT_ENCODING_VALUE + CRLF +
    CONTENT_TYPE + BLANK + CONTENT_TYPE_VALUE + CRLF +
    X_REQUEST_WITH + BLANK + X_REQUEST_WITH_VALUE + CRLF +
    CONTENT_LENGTH + BLANK + std::to_string(SENEC_REQUEST.length()) + CRLF +
    ORIGIN + BLANK + ORIGIN_VALUE + CRLF +
    CONNECTION + BLANK + CONNECTION_CLOSE_VALUE + CRLF +
    REFERER + BLANK + REFERER_VALUE + CRLF +
    SEC_FETCH_DEST + BLANK + SEC_FETCH_DEST_VALUE + CRLF +
    SEC_FETCH_MODE + BLANK + SEC_FETCH_MODE_VALUE + CRLF +
    SEC_FETCH_SITE + BLANK + SEC_FETCH_SITE_VALUE + CRLF +
    CRLF +
    SENEC_REQUEST;
// clang-format on
const std::string P_TOTAL_PCC = "PM1OBJ1.P_TOTAL";
const std::string P_TOTAL_PV = "PM1OBJ2.P_TOTAL";

class SenecDataAcquisition {
public:
  SenecDataAcquisition(boost::asio::io_context &ioContext,
                       boost::asio::ssl::context &sslContext,
                       unsigned TimerDuration);
  SenecDataAcquisition(SenecDataAcquisition &&) = delete;
  SenecDataAcquisition(const SenecDataAcquisition &) = delete;

  void Aquire();
  void
  ResolveHandler(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::resolver::results_type &endpoints);
  void ConnectHandler(const boost::system::error_code &ec);
  void HandshakeHandler(const boost::system::error_code &ec);
  void WriteRequestHandler(const boost::system::error_code &ec);
  void ReadStatusHandler(const boost::system::error_code &ec);
  void ReadHeaderHandler(const boost::system::error_code &ec);
  void ReadContentHandler(const boost::system::error_code &ec);
  void ShutdownHandler(const boost::system::error_code &ec);
  void ProcessResponse();
  ConversionResultOpt GetGridPower() const;
  ConversionResultOpt GetGeneratorPower() const;
  SenecResultSubject &GetResultSubject();

private:
  const unsigned short mPort = 80;
  boost::asio::ip::tcp::endpoint mEndpoint;
  boost::asio::io_context &mrIoContext;
  boost::asio::ssl::context &mrSslContext;
  boost::asio::ip::tcp::resolver mResolver;
  boost::asio::ip::tcp::socket mTcpSocket; // ToDo: remove
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> mSslSocket;
  ptree mTree;
  unsigned mTimerDuration;
  boost::asio::steady_timer mTimer;
  boost::asio::streambuf mRequest;
  boost::asio::streambuf mResponse;
  SenecResultSubject mrSubject;
  my_logger::logger_type &mrLogger;
};

} // namespace SDA