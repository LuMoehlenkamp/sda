#include "senecDataAcquisition.hh"
#include "conversion.hh"
#include "converter.hh"

using namespace boost::asio;
using namespace SDA;

SenecDataAcquisition::SenecDataAcquisition()
    : mEndpoint(ip::address::from_string(mIp), mPort), mResolver(mIoService), mTcpSocket(mIoService, mEndpoint.protocol())
{
}

int SenecDataAcquisition::operator()()
{
  try
  {
    // boost::asio::ip::tcp::resolver::query Query("192.168.178.40","80");
    // mResolver.async_resolve(Query, std::bind(&SenecDataAcquisition::ResolveHandler,this,std::placeholders::_1,std::placeholders::_2));

    // ToDo: make the request a constant
    mTcpSocket.async_connect(mEndpoint, std::bind(&SenecDataAcquisition::ConnectHandler, this, std::placeholders::_1));
    std::stringstream getRequest;
    getRequest << SDA::POST << SDA::BLANK << SDA::SENEC_PATH << SDA::BLANK << SDA::HTTP_VERSION << SDA::CRLF;
    getRequest << SDA::HOST << SDA::BLANK << SDA::HTTP + SDA::SENEC_IP << SDA::CRLF;
    getRequest << SDA::USER_AGENT << SDA::BLANK << SDA::USER_AGENT_VALUE << SDA::CRLF;
    getRequest << SDA::CONTENT_TYPE << SDA::BLANK << SDA::CONTENT_TYPE_VALUE << SDA::CRLF;
    getRequest << SDA::CONTENT_LENGTH << SDA::BLANK << SDA::SENEC_REQUEST.length() << SDA::CRLF;
    getRequest << SDA::ACCEPT << SDA::BLANK << SDA::ACCEPT_VALUE << SDA::CRLF;
    getRequest << SDA::CONNECTION << SDA::BLANK << SDA::CONNECTION_VALUE << CRLF;
    getRequest << SDA::CRLF;
    getRequest << SDA::SENEC_REQUEST;
    // std::cout << getRequest.str() << '\n' << '\n';

    write(mTcpSocket, buffer(getRequest.str()));
    mDataBuffer = {0};
    mTcpSocket.async_read_some(buffer(mDataBuffer),
                               std::bind(&SenecDataAcquisition::ReadHandler,
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
  return 0;
}

void SenecDataAcquisition::ResolveHandler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::results_type results)
{
  if (ec)
  {
    std::cout << "Resolve failed with message: " << ec.message() << '\n';
  }
  else
  {
    // ip::tcp::endpoint endpoint(*results);

    std::cout << "Resolved address: " << '\n'; // endpoint.address() << ':' << endpoint.port() << '\n';
  }
}

void SenecDataAcquisition::ConnectHandler(const boost::system::error_code &ec)
{
  if (ec)
  {
    std::cout << "Connect failed with message: " << ec.message() << '\n';
  }
  else
  {
    std::cout << "Connection established!" << '\n';
    mResponse.clear();
  }
}

void SenecDataAcquisition::ReadHandler(const boost::system::error_code &ec, size_t amountOfBytes)
{
  if (ec)
  {
    if (ec.message() != "End of file")
      std::cout << "\nRead failed with msg: " << ec.message() << '\n';
  }
  else
  {
    std::cout << '\n';
    std::cout << "Read " << amountOfBytes << " Bytes." << '\n';
    for (auto it : mDataBuffer)
    {
      if (it != '\000')
        mResponse.push_back(it);
    }
    mDataBuffer = {0};
    // std::cout.write(mDataBuffer.data(), amountOfBytes);
    mTcpSocket.async_read_some(buffer(mDataBuffer),
                               std::bind(&SenecDataAcquisition::ReadHandler,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }
}

void SenecDataAcquisition::ProcessResponse()
{

  split_vector_type SplitVec; // #2: Search for tokens
  // std::cout << "mResponse: " << mResponse << '\n';
  boost::split(SplitVec, mResponse, boost::is_any_of("'\r\n'"), boost::token_compress_on);
  std::cout << '\n'
            << "Buffer splitted into " << SplitVec.size() << " vectors." << '\n';
  
  std::stringstream ss;
  ss.clear();
  ss << SplitVec[SplitVec.size() - 1].c_str();
  std::cout << ss.str() << '\n';

  ptree pt;
  // variantTree pt;
  boost::property_tree::read_json(ss, pt);
  std::cout << "size: " << pt.size() << '\n';
  for (auto& p : pt)
  {
    std::cout << p.first.c_str() << p.second.get_value<std::string>() << '\n';
  }
  auto x = pt.find("PM1OBJ1");

  // ToDo: iterate over elements
  // ToDo: store converted elements into separate prop_tree
  std::cout << (*x).first << '\n';
  std::string test = pt.get<std::string>("PM1OBJ1.FREQ");
  // ToDo: conversion algo
  std::cout << "test: " << test << '\n';
  float y = Conversion::ConvertToFloat(test);
  std::cout << "y: " << y << '\n';

  std::cout << "Variant:" << '\n';
  boost::variant< uint, float> var(1.2f);
  // boost::variant< uint, float> var(1u);
  // Extra xtra("abc");
  boost::apply_visitor(Converter(), var);
  ConversionResult cr = Conversion::Convert(test);
  std::cout<< "cr: " << cr << '\n';
}

  // std::ofstream out("response.json");
  // out << SplitVec[SplitVec.size()-1].c_str();
  // out.close();