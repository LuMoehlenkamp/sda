#include "solarDataAcquisition.hh"

#include <boost/bind/bind.hpp>

#include <chrono>
#include <fstream>

using namespace boost;
using namespace boost::asio;
using namespace boost::placeholders;
using namespace SDA;

SolarDataAcquisition::SolarDataAcquisition(io_context &ioContext,
                                           unsigned int TimerDuration)
    : mrIoContext(ioContext), mResolver(mrIoContext), mTcpSocket(mrIoContext),
      mTimerDuration(TimerDuration),
      mTimer(ioContext, std::chrono::seconds(INITIAL_TIMER_DURATION)),
      mrLogger(my_logger::get()) {
  mTimer.async_wait(bind(&SolarDataAcquisition::Acquire, this));
}

void SolarDataAcquisition::Acquire() {
  try {
    ip::tcp::resolver::query Query(SOLAR_HOST, "80");
    mResolver.async_resolve(Query, bind(&SolarDataAcquisition::ResolveHandler,
                                        this, boost::asio::placeholders::error,
                                        boost::asio::placeholders::results));
    mTimer.expires_after(std::chrono::seconds(mTimerDuration));
    mTimer.async_wait(boost::bind(&SolarDataAcquisition::Acquire, this));
  } catch (boost::system::system_error &e) {
    std::cerr << e.what() << '\n';
  }
}

void SolarDataAcquisition::ResolveHandler(
    const boost::system::error_code &ec,
    const ip::tcp::resolver::results_type &endpoints) {
  if (ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::ResolveHandler: Resolve failed with message: "
        << ec.message();
  } else {
    async_connect(mTcpSocket, endpoints,
                  bind(&SolarDataAcquisition::ConnectHandler, this,
                       boost::asio::placeholders::error));
  }
}

void SolarDataAcquisition::ConnectHandler(const system::error_code &ec) {
  std::istream response_stream(&mResponse);
  response_stream.clear();
  if (!ec) {
    std::ostream req_stream(&mRequest);
    req_stream << SDA::GET_REQUEST;
    async_write(mTcpSocket, mRequest,
                bind(&SolarDataAcquisition::WriteRequestHandler, this,
                     boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::ConnectHandler: Connect failed with message: "
        << ec.message();
  }
}

void SolarDataAcquisition::WriteRequestHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    async_read_until(mTcpSocket, mResponse, SDA::CRLF,
                     bind(&SolarDataAcquisition::ReadStatusHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::WriteRequestHandler failed with message: "
        << ec.message();
  }
}

void SolarDataAcquisition::ReadStatusHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    std::istream response_stream(&mResponse);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      BOOST_LOG_SEV(mrLogger, severity_level::error)
          << "SolarDataAcquisition::ReadStatusHandler: Invalid response";
      return;
    }
    if (status_code != HTTP_OK_STATUS) {
      BOOST_LOG_SEV(mrLogger, severity_level::error)
          << "SolarDataAcquisition::ReadStatusHandler: Response returned with "
             "status code: "
          << status_code;
      return;
    }
    async_read_until(mTcpSocket, mResponse, "\r\n\r\n",
                     bind(&SolarDataAcquisition::ReadHeaderHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::ReadStatusHandler: Error: " << ec.message();
  }
}

void SolarDataAcquisition::ReadHeaderHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    std::istream response_stream(&mResponse);
    std::string header;
    while (header != "\r") {
      std::getline(response_stream, header);
    }
    async_read(mTcpSocket, mResponse, transfer_at_least(1),
               bind(&SolarDataAcquisition::ReadContentHandler, this,
                    boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::ReadHeaderHandler: Error: " << ec.message();
  }
}

void SolarDataAcquisition::ReadContentHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    async_read(mTcpSocket, mResponse, transfer_at_least(1),
               bind(&SolarDataAcquisition::ReadContentHandler, this,
                    boost::asio::placeholders::error));
  } else if (ec != boost::asio::error::eof) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SolarDataAcquisition::ReadContentHandler: Error: " << ec.message();
  } else {
    ProcessResponse();
  }
}

void SolarDataAcquisition::ProcessResponse() {
  std::istream response_stream(&mResponse);
  std::string response((std::istreambuf_iterator<char>(&mResponse)),
                       std::istreambuf_iterator<char>());

  split_vector_type SplitVec;
  boost::split(SplitVec, response, boost::is_any_of("'\r\n'"),
               boost::token_compress_on);
  for (auto &it : SplitVec) {
    if (it.find("xl6625883") != it.npos) {
      boost::algorithm::trim(it);
      split_vector_type SplitLine;
      boost::split(SplitLine, it, boost::is_any_of("><"),
                   boost::token_compress_on);
      if (SplitLine.size() > 2) {
        std::string str = SplitLine.at(SplitLine.size() - 2);
        if (str == "/td") {
          std::string &float_str = SplitLine.at(SplitLine.size() - 3);
          auto x = std::find(float_str.begin(), float_str.end(), ',');
          std::string dot(".");
          float_str.replace(x, x + 1, dot.c_str());
          std::cout << std::stof(float_str) << '\n';
        }
      }
    }
  }
}