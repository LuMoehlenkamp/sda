#include "senecDataAcquisition.hh"
#include "senecResultDto.hh"
#include "conversion.hh"
#include "converter.hh"

#include <fstream>
#include <chrono>

#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::placeholders;
using namespace SDA;

SenecDataAcquisition::SenecDataAcquisition(io_context& ioContext, unsigned int TimerDuration)
    : mEndpoint(ip::address::from_string(SENEC_IP), mPort)
    , mrIoContext(ioContext)
    , mResolver(mrIoContext)
    , mTcpSocket(mrIoContext, mEndpoint.protocol())
    , mTimerDuration(TimerDuration)
    , mTimer(ioContext, chrono::seconds(mTimerDuration))
{
  mTimer.async_wait(bind(&SenecDataAcquisition::Aquire, this));
}

void SenecDataAcquisition::Aquire()
{
  try
  {
    ip::tcp::resolver::query Query(SENEC_IP,"80");
    mDataBuffer = {0};
    mResolver.async_resolve(SENEC_IP, "http",
      bind(&SenecDataAcquisition::ResolveHandler, 
           this, 
           boost::asio::placeholders::error,
           boost::asio::placeholders::results));
    mTimer.expires_after(chrono::seconds(mTimerDuration));
    mTimer.async_wait(boost::bind(&SenecDataAcquisition::Aquire, this));
  }
  catch (boost::system::system_error &e)
  {
    std::cerr << e.what() << '\n';
  }
}

void SenecDataAcquisition::ResolveHandler(const boost::system::error_code& ec,
                                          const ip::tcp::resolver::results_type& endpoints)
{
  if (ec)
  {
    std::cout << "Resolve failed with message: " << ec.message() << '\n';
  }
  else
  {
    ip::tcp::endpoint endpoint = *endpoints;
    // std::cout << "Resolved address: " << endpoint.address() << ':' << endpoint.port() << '\n';
    async_connect(mTcpSocket, endpoints,
      bind(&SenecDataAcquisition::ConnectHandler,
           this,
           boost::asio::placeholders::error));
  }
}

void SenecDataAcquisition::ConnectHandler(const system::error_code &ec)
{
  if (!ec)
  {
    // std::cout << "Connection established!" << '\n';
    std::ostream req_stream(&mRequest);
    req_stream << SDA::POST_REQUEST;
    async_write(mTcpSocket, mRequest,
      bind(&SenecDataAcquisition::WriteRequestHandler,
           this,
           boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Connect failed with message: " << ec.message() << '\n';
  }
}

void 
SenecDataAcquisition::WriteRequestHandler(const boost::system::error_code& ec)
{
  if (!ec)
  {
    mResponse.prepare(1024);
    async_read_until(mTcpSocket, mResponse, SDA::CRLF,
      bind(&SenecDataAcquisition::ReadStatushandler,
      this,
      boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << ec.message() << '\n';
  }  
}

void 
SenecDataAcquisition::ReadStatushandler(const boost::system::error_code& ec)
{
  if (!ec)
  {
    std::istream response_stream(&mResponse);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response\n";
      return;
    }
    if (status_code != HTTP_OK_STATUS)
    {
      std::cout << "Response returned with status code ";
      std::cout << status_code << "\n";
      return;
    }
    // std::cout << http_version << '\n';
    async_read_until(mTcpSocket, mResponse, "\r\n\r\n",
      bind(&SenecDataAcquisition::ReadHeaderHandler,
           this,
           boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << ec.message() << "\n";
  }
}

void
SenecDataAcquisition::ReadHeaderHandler(const boost::system::error_code& ec)
{
  if (!ec)
  {
    std::istream response_stream(&mResponse);
    std::string header;
    std::getline(response_stream, header);
    // while (std::getline(response_stream, header) && header != "\r")
    //   std::cout << header << '\n';
    // std::cout << '\n';

    boost::asio::async_read(mTcpSocket, mResponse,
      transfer_at_least(1),
      bind(&SenecDataAcquisition::ReadContentHandler,
           this,
           boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << ec.message() << '\n';
  }
}

void
SenecDataAcquisition::ReadContentHandler(const boost::system::error_code& ec)
{
  if (!ec)
  {
    boost::asio::async_read(mTcpSocket, mResponse,
      transfer_at_least(1),
      bind(&SenecDataAcquisition::ReadContentHandler,
           this,
           boost::asio::placeholders::error));
  }
  else
  if (ec.message() != EOF_MESSAGE)
  {
    std::cout << "Error: " << ec.message() << '\n';
  }
  else
  {
    ProcessResponse();
  }
}

void
SenecDataAcquisition::ProcessResponse()
{
  if (mResponse.size() == 0)
  {
    std::cout << "empty content string!" << '\n';
    return;
  }
  mTree.clear();
  std::istream response_stream(&mResponse);
  boost::property_tree::read_json(response_stream, mTree);
  std::string filename = "test.json";
  std::fstream f_stream(filename, f_stream.binary | f_stream.trunc | f_stream.in | f_stream.out);
  boost::property_tree::write_json(f_stream, mTree);
  senecResultDto resultDto;

  auto time_of_measurement = std::chrono::system_clock::now();
  time_t tt;
  tt = std::chrono::system_clock::to_time_t ( time_of_measurement);
  std::cout << "meas time: " << ctime(&tt);
  resultDto.mTimeOfMeasurement = time_of_measurement;

  ConversionResultOpt grid_power_cr = GetGridPower();
  if (grid_power_cr)
  {
    std::cout<< "grid power: " << grid_power_cr.get() << '\n';
    resultDto.mPowerGrid = boost::get<float>(grid_power_cr.get());
  }

  ConversionResultOpt generator_power_cr = GetGeneratorPower();
  if (generator_power_cr)
  {
    std::cout << "generator power: " << generator_power_cr.get() << '\n';
    resultDto.mPowerGen = boost::get<float>(generator_power_cr.get());
  }

  std::string house_power = mTree.get<std::string>("ENERGY.GUI_HOUSE_POW");
  ConversionResultOpt house_power_cr = Conversion::Convert(house_power);
  if (house_power_cr)
  {
    std::cout<< "house_consumption: " << house_power_cr.get() << '\n';
    resultDto.mPowerHouse = boost::get<float>(house_power_cr.get());
  }

  std::string bat_power = mTree.get<std::string>("ENERGY.GUI_BAT_DATA_POWER");
  ConversionResultOpt bat_power_cr = Conversion::Convert(bat_power);
  if (bat_power_cr)
  {
    std::cout<< "bat power: " << bat_power_cr.get() << '\n';
    resultDto.mPowerBat = boost::get<float>(bat_power_cr.get());
  }

  std::string bat_charge = mTree.get<std::string>("ENERGY.GUI_BAT_DATA_FUEL_CHARGE");
  ConversionResultOpt bat_charge_cr = Conversion::Convert(bat_charge);
  if (bat_charge_cr)
  {
    std::cout<< "bat charge: " << bat_charge_cr.get() << '\n';
    resultDto.mChargingLevel = boost::get<float>(bat_charge_cr.get());
  }

  std::cout << '\n';

}

ConversionResultOpt 
SenecDataAcquisition::GetGridPower() const
{
  std::string power = mTree.get<std::string>(SDA::P_TOTAL_PCC);
  ConversionResultOpt power_cr = Conversion::Convert(power);
  return power_cr;
}

ConversionResultOpt
SenecDataAcquisition::GetGeneratorPower() const
{
  std::string power = mTree.get<std::string>(SDA::P_TOTAL_PV);
  ConversionResultOpt power_cr = Conversion::Convert(power);
  return power_cr; 
}
