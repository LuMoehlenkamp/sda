#include "senecDataAcquisition.hh"
#include "conversion.hh"
#include "senecResultDto.hh"

#include <chrono>
#include <fstream>

#include <boost/bind/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/version.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ssl;
using namespace boost::placeholders;
using namespace SDA;

SenecDataAcquisition::SenecDataAcquisition(io_context &ioContext,
                                           context &sslContext,
                                           unsigned int TimerDuration)
    : mEndpoint(ip::address::from_string(SENEC_IP), mPort),
      mrIoContext(ioContext), mResolver(mrIoContext), mrSslContext(sslContext),
      mTcpSocket(mrIoContext, mEndpoint.protocol()),
      mSslSocket(mrIoContext, mrSslContext), mTimerDuration(TimerDuration),
      mTimer(ioContext, std::chrono::seconds(INITIAL_TIMER_DURATION)),
      mrLogger(my_logger::get()) {
  mSslSocket.set_verify_mode(boost::asio::ssl::verify_none);
  mTimer.async_wait(bind(&SenecDataAcquisition::Acquire, this));
}

void SenecDataAcquisition::Acquire() {
  try {
    mSslSocket.set_verify_mode(boost::asio::ssl::verify_none);
    ip::tcp::resolver::query Query(SENEC_IP, "443");
    mResolver.async_resolve(SENEC_IP, "https",
                            bind(&SenecDataAcquisition::ResolveHandler, this,
                                 boost::asio::placeholders::error,
                                 boost::asio::placeholders::results));
    mTimer.expires_after(std::chrono::seconds(mTimerDuration));
    mTimer.async_wait(boost::bind(&SenecDataAcquisition::Acquire, this));
  } catch (boost::system::system_error &e) {
    std::cerr << e.what() << '\n';
  }
}

void SenecDataAcquisition::ResolveHandler(
    const boost::system::error_code &ec,
    const ip::tcp::resolver::results_type &endpoints) {
  if (ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ResolveHandler: Resolve failed with message: "
        << ec.message();
  } else {
    ip::tcp::endpoint endpoint = *endpoints;
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ResolveHandler: Resolved address: "
        << endpoint.address() << ':' << endpoint.port();
    async_connect(mSslSocket.next_layer(), endpoints,
                  bind(&SenecDataAcquisition::ConnectHandler, this,
                       boost::asio::placeholders::error));
  }
}

void SenecDataAcquisition::ConnectHandler(const system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ConnectHandler: Connection established!";
    std::ostream req_stream(&mRequest);
    req_stream << SDA::POST_REQUEST;
    mSslSocket.async_handshake(boost::asio::ssl::stream_base::client,
                               bind(&SenecDataAcquisition::HandshakeHandler,
                                    this, boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ConnectHandler: Connect failed with message: "
        << ec.message();
  }
}

void SenecDataAcquisition::HandshakeHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::HandshakeHandler: Handshake successful";
    std::ostream req_stream(&mRequest);
    async_write(mSslSocket, mRequest,
                bind(&SenecDataAcquisition::WriteRequestHandler, this,
                     boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::HandshakeHandler: Connect failed with "
           "message: "
        << ec.message();
  }
}

void SenecDataAcquisition::WriteRequestHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::WriteRequestHandler: Request successful "
           "written";
    mResponse.commit(1024);
    mResponse.consume(1024);
    mResponse.prepare(1024);
    async_read_until(mSslSocket, mResponse, "\r\n\r\n",
                     bind(&SenecDataAcquisition::ReadHeaderHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::WriteRequestHandler failed with message: "
        << ec.message();
    boost::system::error_code ec2;
    mSslSocket.lowest_layer().cancel(ec2);
    mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                   boost::asio::placeholders::error));
  }
}

void SenecDataAcquisition::ReadStatusHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ReadStatusHandler: Response status succesful "
           "read";
    // std::istream response_stream(&mResponse);
    // std::string http_version;
    // response_stream >> http_version;
    // unsigned int status_code;
    // response_stream >> status_code;
    // std::string status_message;
    // std::getline(response_stream, status_message);
    // if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
    //   BOOST_LOG_SEV(mrLogger, severity_level::error)
    //       << "SenecDataAcquisition::ReadStatusHandler: Invalid response";
    //   return;
    // if (status_code != HTTP_OK_STATUS) {
    //   BOOST_LOG_SEV(mrLogger, severity_level::error)
    //       << "SenecDataAcquisition::ReadStatusHandler: Response returned with
    //       "
    //          "status code: "
    //       << status_code;
    //   return;
    // }
    // BOOST_LOG_SEV(mrLogger, severity_level::error)
    //     << "SenecDataAcquisition::ReadStatusHandler: issuind 2nd read
    //     operation"
    //     << "status code: " << status_code;
    async_read_until(mSslSocket, mResponse, "\r\n\r\n",
                     bind(&SenecDataAcquisition::ReadHeaderHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadStatusHandler: Error: " << ec.message();
    boost::system::error_code ec2;
    mSslSocket.lowest_layer().cancel(ec2);
    mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                   boost::asio::placeholders::error));
    // std::ostringstream oss;
    // oss << &mResponse;
    // std::cout << oss.str() << std::endl;
  }
}

void SenecDataAcquisition::ReadHeaderHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ReadHeaderHandler: Response header "
           "successful read";
    std::istream response_stream(&mResponse);
    std::string header_1, header_2, header_3, header_4, header_5, header_6;
    std::getline(response_stream, header_1);
    std::getline(response_stream, header_2);
    std::getline(response_stream, header_3);
    std::getline(response_stream, header_4);
    std::getline(response_stream, header_5);
    std::getline(response_stream, header_5);
    std::getline(response_stream, header_6);

    std::cout << "header:" << '\n'
              << header_1 << '\n'
              << header_2 << '\n'
              << header_3 << '\n'
              << header_4 << '\n'
              << header_5 << '\n'
              << header_6 << '\n'
              << "end of header" << '\n';
    async_read_until(mSslSocket, mResponse, EOF_MESSAGE,
                     bind(&SenecDataAcquisition::ReadContentHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadHeaderHandler: Error: " << ec.message();
    boost::system::error_code ec2;
    mSslSocket.lowest_layer().cancel(ec2);
    mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                   boost::asio::placeholders::error));
  }
}

void SenecDataAcquisition::ReadContentHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ReadContentHandler: reading content";
    ProcessResponse();
    // async_read_until(mSslSocket, mResponse, transfer_at_least(1),
    //                  bind(&SenecDataAcquisition::ReadContentHandler, this,
    //                       boost::asio::placeholders::error));
    // } else if (ec.message() != EOF_MESSAGE) {
    //   BOOST_LOG_SEV(mrLogger, severity_level::error)
    //       << "SenecDataAcquisition::ReadContentHandler: Error: " <<
    //       ec.message();
    //   mResponse.consume(mResponse.size());
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadContentHandler: Error: " << ec.message();
    mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                   boost::asio::placeholders::error));
    // mResponse.consume(mResponse.size());
  }
}

void SenecDataAcquisition::ShutdownHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ShutdownHandler: ssl shutdown successful";
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::normal)
        << "SenecDataAcquisition::ShutdownHandler: ssl shutdown reported "
           "error: "
        << ec.message();
  }
  boost::system::error_code ec2;
  mSslSocket.lowest_layer().close(ec2);
}

void SenecDataAcquisition::ProcessResponse() {
  if (mResponse.size() == 0) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ProcessResponse: empty content string";
    boost::system::error_code ec2;
    mSslSocket.lowest_layer().cancel(ec2);
    mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                   boost::asio::placeholders::error));
    return;
  }
  BOOST_LOG_SEV(mrLogger, severity_level::error)
      << "SenecDataAcquisition::ProcessResponse: processing response";
  mTree.clear();
  std::istream response_stream(&mResponse);
  std::string line_1;
  std::getline(response_stream, line_1);
  std::cout << "removed: " << '\n'
            << line_1
            << '\n'
            // << line_2 << '\n'
            << "end of removed" << '\n';
  std::string json_response;
  std::getline(response_stream, json_response);
  std::string line_2;
  std::string line_3;
  std::getline(response_stream, line_2);
  std::getline(response_stream, line_3);
  // std::cout << "json: " << json_response << '\n' << "end of json" << '\n';
  std::istringstream json_stream(json_response);

  try {
    boost::property_tree::read_json(json_stream, mTree);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  SenecResultDto resultDto;

  auto time_of_measurement = std::chrono::system_clock::now();
  resultDto.mTimeOfMeasurement = time_of_measurement;

  try {
    ConversionResultOpt grid_power_cr = GetGridPower();
    if (grid_power_cr) {
      resultDto.mPowerGrid = boost::get<float>(grid_power_cr.get());
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  try {
    ConversionResultOpt generator_power_cr = GetGeneratorPower();
    if (generator_power_cr) {
      resultDto.mPowerGen = boost::get<float>(generator_power_cr.get());
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  std::string house_power = mTree.get<std::string>("ENERGY.GUI_HOUSE_POW");
  ConversionResultOpt house_power_cr = Conversion::Convert(house_power);
  if (house_power_cr) {
    resultDto.mPowerHouse = boost::get<float>(house_power_cr.get());
  }

  std::string bat_power = mTree.get<std::string>("ENERGY.GUI_BAT_DATA_POWER");
  ConversionResultOpt bat_power_cr = Conversion::Convert(bat_power);
  if (bat_power_cr) {
    resultDto.mPowerBat = boost::get<float>(bat_power_cr.get());
  }

  std::string bat_charge =
      mTree.get<std::string>("ENERGY.GUI_BAT_DATA_FUEL_CHARGE");
  ConversionResultOpt bat_charge_cr = Conversion::Convert(bat_charge);
  if (bat_charge_cr) {
    resultDto.mChargeLevel = boost::get<float>(bat_charge_cr.get());
  }

  mrSubject.Notify(resultDto);
  BOOST_LOG_SEV(mrLogger, severity_level::error)
      << "SenecDataAcquisition::ProcessResponse: SenecResultSubject notified";

  boost::system::error_code ec;
  mSslSocket.lowest_layer().cancel(ec);
  mSslSocket.async_shutdown(bind(&SenecDataAcquisition::ShutdownHandler, this,
                                 boost::asio::placeholders::error));
}

ConversionResultOpt SenecDataAcquisition::GetGridPower() const {
  std::string power = mTree.get<std::string>(SDA::P_TOTAL_PCC);
  ConversionResultOpt power_cr = Conversion::Convert(power);
  return power_cr;
}

ConversionResultOpt SenecDataAcquisition::GetGeneratorPower() const {
  std::string power = mTree.get<std::string>(SDA::P_TOTAL_PV);
  ConversionResultOpt power_cr = Conversion::Convert(power);
  return power_cr;
}

SenecResultSubject &SenecDataAcquisition::GetResultSubject() {
  return mrSubject;
}