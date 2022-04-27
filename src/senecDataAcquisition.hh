#pragma once

#include <array>
#include <iostream>
#include <string>
#include <istream>
#include <functional>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <utility>
#include <map>

#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/variant.hpp>

namespace SDA
{
  const std::string POST = "POST";
  const std::string BLANK = " ";
  const std::string SENEC_PATH = "/lala.cgi";
  const std::string HTTP_VERSION = "HTTP/1.1";
  const std::string CRLF = "\r\n";
  const std::string HOST = "Host:";
  const std::string HTTP = "http://";
  const std::string SENEC_IP = "192.168.178.40";
  const std::string USER_AGENT = "User-Agent:";
  const std::string USER_AGENT_VALUE = "C/1.0";
  const std::string CONTENT_TYPE = "Content-Type:";
  const std::string CONTENT_TYPE_VALUE = "application/json; charset=utf-8";
  const std::string CONTENT_LENGTH = "Content-Length:";
  const std::string SENEC_REQUEST = "{\"PM1OBJ1\":{}, \"PM1OBJ2\":{}, \"BAT1OBJ1\":{}, \"STATISTIC\":{}}";
  const std::string ACCEPT = "Accept:";
  const std::string ACCEPT_VALUE = "*/*";
  const std::string CONNECTION = "Connection:";
  const std::string CONNECTION_VALUE = "close";

  // const std::string SENEC_REQUEST = "{\"PM1OBJ1\":{\"FREQ\":\"\",\"U_AC\":\"\",\"I_AC\":\"\",\"P_AC\":\"\",\"P_TOTAL\":\"\"}}";
  const std::string POST_REQUEST = POST + BLANK + SENEC_PATH + BLANK + HTTP_VERSION + CRLF +
                                   HOST + BLANK + HTTP + SENEC_IP + CRLF +
                                   USER_AGENT + BLANK + USER_AGENT_VALUE + CRLF +
                                   CONTENT_TYPE + BLANK + CONTENT_TYPE_VALUE + CRLF +
                                   CONTENT_LENGTH + BLANK + std::to_string(SENEC_REQUEST.length()) + CRLF +
                                   ACCEPT + BLANK + ACCEPT_VALUE + CRLF +
                                   CONNECTION + BLANK + CONNECTION_VALUE + CRLF +
                                   CRLF +
                                   SENEC_REQUEST;

  const std::string P_TOTAL_PCC = "PM1OBJ1.P_TOTAL";
  const std::string P_TOTAL_PV = "PM1OBJ2.P_TOTAL";

  typedef boost::property_tree::basic_ptree<std::string, std::string> ptree;
  typedef boost::variant<uint, float> ConversionResult;
  typedef boost::optional<ConversionResult> ConversionResultOpt;
  typedef std::vector<std::string> split_vector_type;
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
    const std::string mIp = "192.168.178.40";
    const unsigned short mPort = 80;
    const std::string mPortAsString = "80";
    boost::asio::ip::tcp::endpoint mEndpoint;
    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mTcpSocket;
    std::array<char, 1024> mDataBuffer = {0};
    std::string mResponse;
    ptree mTree;
  };

}