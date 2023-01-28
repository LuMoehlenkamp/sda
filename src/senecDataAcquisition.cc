#include "senecDataAcquisition.hh"
#include "conversion.hh"
#include "senecResultDto.hh"

#include <chrono>
#include <fstream>

#include <boost/bind/bind.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::placeholders;
using namespace SDA;

SenecDataAcquisition::SenecDataAcquisition(io_context &ioContext,
                                           unsigned int TimerDuration)
    : mEndpoint(ip::address::from_string(SENEC_IP), mPort),
      mrIoContext(ioContext), mResolver(mrIoContext),
      mTcpSocket(mrIoContext, mEndpoint.protocol()),
      mTimerDuration(TimerDuration),
      mTimer(ioContext, std::chrono::seconds(INITIAL_TIMER_DURATION)),
      mrLogger(my_logger::get()) {
  mTimer.async_wait(bind(&SenecDataAcquisition::Aquire, this));
}

void SenecDataAcquisition::Aquire() {
  try {
    ip::tcp::resolver::query Query(SENEC_IP, "80");
    mResolver.async_resolve(SENEC_IP, "http",
                            bind(&SenecDataAcquisition::ResolveHandler, this,
                                 boost::asio::placeholders::error,
                                 boost::asio::placeholders::results));
    mTimer.expires_after(std::chrono::seconds(mTimerDuration));
    mTimer.async_wait(boost::bind(&SenecDataAcquisition::Aquire, this));
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
    async_connect(mTcpSocket, endpoints,
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
    async_write(mTcpSocket, mRequest,
                bind(&SenecDataAcquisition::WriteRequestHandler, this,
                     boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ConnectHandler: Connect failed with message: "
        << ec.message();
  }
}

void SenecDataAcquisition::WriteRequestHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    mResponse.prepare(1024);
    async_read_until(mTcpSocket, mResponse, SDA::CRLF,
                     bind(&SenecDataAcquisition::ReadStatusHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::WriteRequestHandler failed with message: "
        << ec.message();
  }
}

void SenecDataAcquisition::ReadStatusHandler(
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
          << "SenecDataAcquisition::ReadStatusHandler: Invalid response";
      return;
    }
    if (status_code != HTTP_OK_STATUS) {
      BOOST_LOG_SEV(mrLogger, severity_level::error)
          << "SenecDataAcquisition::ReadStatusHandler: Response returned with "
             "status code: "
          << status_code;
      return;
    }
    async_read_until(mTcpSocket, mResponse, "\r\n\r\n",
                     bind(&SenecDataAcquisition::ReadHeaderHandler, this,
                          boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadStatusHandler: Error: " << ec.message();
  }
}

void SenecDataAcquisition::ReadHeaderHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    std::istream response_stream(&mResponse);
    std::string header;
    std::getline(response_stream, header);
    boost::asio::async_read(mTcpSocket, mResponse, transfer_at_least(1),
                            bind(&SenecDataAcquisition::ReadContentHandler,
                                 this, boost::asio::placeholders::error));
  } else {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadHeaderHandler: Error: " << ec.message();
  }
}

void SenecDataAcquisition::ReadContentHandler(
    const boost::system::error_code &ec) {
  if (!ec) {
    boost::asio::async_read(mTcpSocket, mResponse, transfer_at_least(1),
                            bind(&SenecDataAcquisition::ReadContentHandler,
                                 this, boost::asio::placeholders::error));
  } else if (ec.message() != EOF_MESSAGE) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ReadContentHandler: Error: " << ec.message();
  } else {
    ProcessResponse();
  }
}

void SenecDataAcquisition::ProcessResponse() {
  if (mResponse.size() == 0) {
    BOOST_LOG_SEV(mrLogger, severity_level::error)
        << "SenecDataAcquisition::ProcessResponse: empty content string";
    return;
  }
  mTree.clear();
  std::istream response_stream(&mResponse);
  boost::property_tree::read_json(response_stream, mTree);
  // std::string filename = "test.json";
  // std::fstream f_stream(filename, f_stream.binary | f_stream.trunc |
  //                                     f_stream.in | f_stream.out);
  // boost::property_tree::write_json(f_stream, mTree);
  SenecResultDto resultDto;

  auto time_of_measurement = std::chrono::system_clock::now();
  resultDto.mTimeOfMeasurement = time_of_measurement;

  ConversionResultOpt grid_power_cr = GetGridPower();
  if (grid_power_cr) {
    resultDto.mPowerGrid = boost::get<float>(grid_power_cr.get());
  }

  ConversionResultOpt generator_power_cr = GetGeneratorPower();
  if (generator_power_cr) {
    resultDto.mPowerGen = boost::get<float>(generator_power_cr.get());
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