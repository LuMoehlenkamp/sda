#include <boost/asio.hpp>

#include <gpiod.h>
#include <wiringPi.h>

#include "senecDataAcquisition.hh"
#include "solarDataAcquisition.hh"

#include "configManager.hh"

#include "senecResultObserver.hh"

int main(int argc, char *argv[])
{
  SDA::ConfigManager* p_config_manager = SDA::ConfigManager::GetInstance(SDA::CONFIG_PATH);
  auto testmode = p_config_manager->GetTestMode();
  auto senec_update_time = p_config_manager->GetSenecUpdateTime();
  auto solar_update_time = p_config_manager->GetSolarUpdateTime();

  if (!testmode)
  {
    if (wiringPiSetup () == -1)
    {
      std::cout << "wiringpi init unsuccessful... goodbye!" << '\n';
      exit (1) ;
    }
  }

  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work =
    boost::asio::make_work_guard(ioContext);

  SDA::SenecDataAcquisition senec_da(ioContext, senec_update_time);
  SDA::SolarDataAcquisition solar_da(ioContext, solar_update_time);

  auto& senec_rs = senec_da.GetResultSubject();
  SDA::SenecResultObserver observer(senec_rs);

  ioContext.run();
  
  return 0;
}