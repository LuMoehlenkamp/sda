#include "global.hh"
#include <wiringPi.h>

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