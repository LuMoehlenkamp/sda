#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/move/utility_core.hpp>

// #include <pigpiod_if2.h>
#include "configManager.hh"
#include "powerControl.hh"
#include "senecDataAcquisition.hh"
#include "senecResultObserver.hh"
#include "solarDataAcquisition.hh"
#include <gpiod.h>
// #include <wiringPi.h>

enum severity_level { normal, notification, warning, error, critical };

typedef boost::log::sources::severity_channel_logger_mt<severity_level,
                                                        std::string>
    my_logger_mt;

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, my_logger_mt) {
  return my_logger_mt(boost::log::keywords::channel = "my_logger");
}

void InitLogger() {
  boost::log::add_file_log(
      boost::log::keywords::file_name = "sample_%N.log",
      boost::log::keywords::target_file_name = "sample_%N.log",
      boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::time_based_rotation =
          boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::format = "[%TimeStamp%]: %Message%",
      boost::log::keywords::auto_flush = true);

  boost::log::add_common_attributes();
}

int main(int argc, char *argv[]) {
  InitLogger();
  auto chip = gpiod_chip_open_by_name("gpiochip0");
  auto lineRed = gpiod_chip_get_line(chip, 24);

  auto &logger = my_logger::get();
  // BOOST_LOG(logger) << "Greetings from the global logger!";
  BOOST_LOG_SEV(my_logger::get(), normal) << "application started";
  // BOOST_LOG_SEV(my_logger::get(), error) << "Hello, error world!";
  // BOOST_LOG_CHANNEL_SEV(logger, "test", critical) << "Hello, critical
  // world!";
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
    // if (wiringPiSetup() == -1) {
    //   std::cout << "wiringpi init unsuccessful... goodbye!" << '\n';
    //   exit(1);
    // }
  }

  boost::asio::io_context ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(ioContext);

  SDA::SenecDataAcquisition senec_da(ioContext, senec_update_time_opt.get());
  SDA::SolarDataAcquisition solar_da(ioContext, solar_update_time_opt.get());
  auto &senec_rs = senec_da.GetResultSubject();
  SDA::PowerControl power_control(ioContext, power_control_cycle_time_opt.get(),
                                  senec_rs);

  SDA::SenecResultObserver observer(senec_rs);

  ioContext.run();

  return 0;
}