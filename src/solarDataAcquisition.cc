#include "solarDataAcquisition.hh"

using namespace boost::asio;
using namespace SDA;

SolarDataAcquisition::SolarDataAcquisition()
    : mEndpoint(), mResolver(mIoService), mTcpSocket(mIoService, mEndpoint.protocol())
{}

int SolarDataAcquisition::operator()()
{
  try
  {
    std::cout << "start connecting solar forecast service with Request: " << '\n' << GET_REQUEST << '\n';
    boost::asio::ip::tcp::resolver::query Query(SOLAR_HOST, "80");
    mDataBuffer = {0};
    mResolver.async_resolve(Query,
                            std::bind(&SolarDataAcquisition::ResolveHandler,
                                      this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
    mIoService.run();
  }
  catch (boost::system::system_error &e)
  {
    std::cerr << e.what() << '\n';
    return e.code().value();
  }
  ProcessResponse();
  mIoService.stop();
  mTcpSocket.close();
  return 0;
}

void SolarDataAcquisition::ResolveHandler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::results_type results)
{
  if (ec)
  {
    std::cout << "Resolve failed with message: " << ec.message() << '\n';
  }
  else
  {
    mEndpoint = *results;
    std::cout << "Resolved address: " << mEndpoint.address() << ':' << mEndpoint.port() << '\n';
    mTcpSocket.async_connect(mEndpoint, std::bind(&SolarDataAcquisition::ConnectHandler, this, std::placeholders::_1));
  }
}

void SolarDataAcquisition::ConnectHandler(const boost::system::error_code &ec)
{
  if (ec)
  {
    std::cout << "Connect failed with message: " << ec.message() << '\n';
  }
  else
  {
    std::cout << "Connection established!" << '\n';
    mResponse.clear();
    write(mTcpSocket, buffer(SDA::GET_REQUEST));
    mDataBuffer = {0};
    mTcpSocket.async_read_some(buffer(mDataBuffer),
                               std::bind( &SolarDataAcquisition::ReadHandler,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
  }
}

void SolarDataAcquisition::ReadHandler(const boost::system::error_code &ec, size_t amountOfBytes)
{
  if (ec)
  {
    if (ec.message() != "End of file")
      std::cout << "\nRead failed with msg: " << ec.message() << '\n'; // ToDo: Replace console output
  }
  else
  {
    for (auto it : mDataBuffer)
    {
      if (it != '\000')
        mResponse.push_back(it);
    }
    mDataBuffer = {0};
    mTcpSocket.async_read_some(buffer(mDataBuffer),
                               std::bind(&SolarDataAcquisition::ReadHandler,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }
}

void SolarDataAcquisition::ProcessResponse()
{
  split_vector_type SplitVec;
  boost::split(SplitVec, mResponse, boost::is_any_of("'\r\n'"), boost::token_compress_on);
  for (auto& it : SplitVec)
  {
    if (it.find("xl6625883") != it.npos)
    {
      boost::algorithm::trim(it);
      split_vector_type SplitLine;
      boost::split(SplitLine, it, boost::is_any_of("><"), boost::token_compress_on);
      if (SplitLine.size() > 2)
      {
        std::string str = SplitLine.at(SplitLine.size()-2);
        if (str == "/td")
        {
          std::string& float_str =  SplitLine.at(SplitLine.size()-3);
          auto x = std::find(float_str.begin(),float_str.end(),',');
          std::string dot(".");
          float_str.replace(x,x+1,dot.c_str());
          std::cout << std::stof(float_str) << '\n';
        }
      }
    }
  }
}