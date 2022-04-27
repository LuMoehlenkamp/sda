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

    mTcpSocket.async_connect(mEndpoint, std::bind(&SenecDataAcquisition::ConnectHandler, this, std::placeholders::_1));
    write(mTcpSocket, buffer(SDA::POST_REQUEST));
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
                               std::bind(&SenecDataAcquisition::ReadHandler,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }
}

void SenecDataAcquisition::ProcessResponse()
{
  split_vector_type SplitVec;
  boost::split(SplitVec, mResponse, boost::is_any_of("'\r\n'"), boost::token_compress_on);
  // std::cout << '\n'
  //           << "Buffer splitted into " << SplitVec.size() << " vectors." << '\n';
  
  std::stringstream ss;
  ss.clear();
  ss << SplitVec[SplitVec.size() - 1].c_str();
  // std::cout << ss.str() << '\n';
  boost::property_tree::read_json(ss, mTree);

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
    std::cout<< "power 2 cr: " << power_2_cr << '\n';
}

ConversionResultOpt SenecDataAcquisition::GetGridPower() const
{
  std::string power_1 = mTree.get<std::string>(SDA::P_TOTAL_PCC);
  std::cout << "P total 1 obj.: " << power_1 << '\n';
  ConversionResultOpt power_1_cr = Conversion::Convert(power_1);
  return power_1_cr;
}

  // std::ofstream out("response.json");
  // out << SplitVec[SplitVec.size()-1].c_str();
  // out.close();

  // float y = Conversion::ConvertToFloat(test);
  // std::cout << "y: " << y << '\n';

  // std::cout << "Variant:" << '\n';
  // boost::variant< uint, float> var(1.2f);
  // boost::apply_visitor(Converter(), var);