#pragma once


#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace SDA
{
  const std::string GET = "GET";
  const std::string POST = "POST";
  const std::string BLANK = " ";
  const std::string CRLF = "\r\n";
  const std::string HTTP_VERSION = "HTTP/1.1";
  const std::string HOST = "Host:";
  const std::string HTTP = "http://";
  const std::string USER_AGENT = "User-Agent:";
  const std::string USER_AGENT_VALUE = "C/1.0";
  const std::string CONTENT_TYPE = "Content-Type:";
  const std::string CONTENT_TYPE_VALUE = "application/json; charset=utf-8";
  const std::string CONTENT_LENGTH = "Content-Length:";
  const std::string ACCEPT = "Accept:";
  const std::string ACCEPT_VALUE = "*/*";
  const std::string CONNECTION = "Connection:";
  const std::string CONNECTION_KEEP_ALIVE_VALUE = "keep-alive";
  const std::string CONNECTION_CLOSE_VALUE = "close";
  const std::string ACCEPT_ENCODING = "Accept-Encoding:";
  const std::string ACCEPT_ENCODING_VALUE = "gzip, deflate";
  const std::string ACCEPT_LANGUAGE = "Accept-Language:";
  const std::string ACCEPT_LANGUAGE_VALUE = "en-US,en;q=0.5";

  const std::string EOF_MESSAGE = "End of file";

  const unsigned int HTTP_OK_STATUS = 200;

  typedef std::vector<std::string> split_vector_type;
  typedef boost::property_tree::basic_ptree<std::string, std::string> ptree;
}