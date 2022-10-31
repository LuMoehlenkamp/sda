#include <boost/asio.hpp>

#include <gpiod.h>
#include <wiringPi.h>

#include "configManager.hh"
#include "powerControl.hh"
#include "senecDataAcquisition.hh"
#include "senecResultObserver.hh"
#include "solarDataAcquisition.hh"


int main(int argc, char *argv[])
{
  SDA::ConfigManager* p_config_manager = SDA::ConfigManager::GetInstance(SDA::CONFIG_PATH);
  
  auto testmode_opt = p_config_manager->GetTestMode();
  auto senec_update_time_opt = p_config_manager->GetSenecUpdateTime();
  auto solar_update_time_opt = p_config_manager->GetSolarUpdateTime();
  auto power_control_cycle_time_opt = p_config_manager->GetPowerControlCycleTime();
  
  bool testmode = true;
  if (testmode_opt)
    testmode = testmode_opt.get();

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

  SDA::SenecDataAcquisition senec_da(ioContext, senec_update_time_opt.get());
  SDA::SolarDataAcquisition solar_da(ioContext, solar_update_time_opt.get());
  auto& senec_rs = senec_da.GetResultSubject();
  SDA::PowerControl power_control(ioContext, 
                                  power_control_cycle_time_opt.get(),
                                  senec_rs);

  SDA::SenecResultObserver observer(senec_rs);

  ioContext.run();
  
  return 0;
}