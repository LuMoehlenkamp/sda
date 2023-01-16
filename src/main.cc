#include "configManager.hh"
#include "global.hh"
#include "powerControl.hh"
#include "senecDataAcquisition.hh"
#include "senecResultObserver.hh"
#include "solarDataAcquisition.hh"

#include <iostream>
#include <wiringPi.h>

int main(int argc, char *argv[]) {
  std::cout << "application started..." << '\n';

  InitLogger();
  auto &logger = my_logger::get();
  BOOST_LOG_SEV(logger, normal) << "application started";

  SDA::ConfigManager *p_config_manager(
      SDA::ConfigManager::GetInstance(SDA::ConfigManager::CONFIG_PATH));
  auto senec_update_time_opt = p_config_manager->GetSenecUpdateTime();
  auto solar_update_time_opt = p_config_manager->GetSolarUpdateTime();

  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(ioContext);

  SDA::SenecDataAcquisition senec_da(ioContext, senec_update_time_opt.get());
  SDA::SolarDataAcquisition solar_da(ioContext, solar_update_time_opt.get());

  auto &senec_rs = senec_da.GetResultSubject();
  SDA::PowerControl power_control(ioContext, senec_rs);

  std::signal(SIGINT, SDA::PowerControl::SignalHandler);
  std::signal(SIGTERM, SDA::PowerControl::SignalHandler);

  ioContext.run();

  return 0;
}