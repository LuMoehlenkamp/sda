#include "senecDataAcquisition.hh"
#include "conversion.hh"
#include "converter.hh"

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
    std::cout << "Resolved address: " << endpoint.address() << ':' << endpoint.port() << '\n';
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
    std::cout << "Connection established!" << '\n';

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
    std::cout << http_version << '\n';
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
    while (std::getline(response_stream, header) && header != "\r")
      std::cout << header << '\n';
    std::cout << '\n';

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

  std::string frequency = mTree.get<std::string>("PM1OBJ1.FREQ");
  std::cout << "frequency obj.: " << frequency << '\n';
  ConversionResultOpt frequency_cr = Conversion::Convert(frequency);
  if (frequency_cr)
    std::cout<< "frequency cr: " << frequency_cr << '\n';

  ConversionResultOpt power_1_cr = GetGridPower();
  if (power_1_cr)
    std::cout<< "power 1 cr: " << power_1_cr << '\n';

  std::string power_2 = mTree.get<std::string>("PM1OBJ2.P_TOTAL");
  std::cout << "P total 2 obj.: " << power_2 << '\n';
  ConversionResultOpt power_2_cr = Conversion::Convert(power_2);
  if (power_2_cr)
    std::cout << "power 2 cr: " << power_2_cr << '\n';
    std::cout << '\n';
}

ConversionResultOpt 
SenecDataAcquisition::GetGridPower() const
{
  std::string power_1 = mTree.get<std::string>(SDA::P_TOTAL_PCC);
  std::cout << "P total 1 obj.: " << power_1 << '\n'; // todo: Remove after testing
  ConversionResultOpt power_1_cr = Conversion::Convert(power_1);
  return power_1_cr;
}