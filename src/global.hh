#pragma once

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/move/utility_core.hpp>

enum severity_level { normal, notification, warning, error, critical };

typedef boost::log::sources::severity_channel_logger_mt<severity_level,
                                                        std::string>
    my_logger_mt;

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, my_logger_mt) {
  return my_logger_mt(boost::log::keywords::channel = "my_logger");
}

void InitLogger();