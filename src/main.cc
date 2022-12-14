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

  SDA::ConfigManager *p_config_manager =
      SDA::ConfigManager::GetInstance(SDA::CONFIG_PATH);
  auto testmode_opt = p_config_manager->GetTestMode();
  auto senec_update_time_opt = p_config_manager->GetSenecUpdateTime();
  auto solar_update_time_opt = p_config_manager->GetSolarUpdateTime();
  auto power_control_cycle_time_opt =
      p_config_manager->GetPowerControlCycleTime();

  bool testmode = true;
  if (testmode_opt)
    testmode = testmode_opt.get();

  if (!testmode) {
    if (wiringPiSetupGpio() == -1) {
      BOOST_LOG_SEV(logger, normal) << "wiringpi init unsuccessful... goodbye!";
      exit(1);
    }
    pinMode(18, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(3840);
    pwmSetRange(1000);
    pwmWrite(18, 1000);
  }

  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(ioContext);

  SDA::SenecDataAcquisition senec_da(ioContext, senec_update_time_opt.get());
  SDA::SolarDataAcquisition solar_da(ioContext, solar_update_time_opt.get());
  auto &senec_rs = senec_da.GetResultSubject();
  SDA::PowerControl power_control(ioContext, power_control_cycle_time_opt.get(),
                                  senec_rs);

  ioContext.run();

  return 0;
}