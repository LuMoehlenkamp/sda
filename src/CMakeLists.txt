find_package(Boost REQUIRED COMPONENTS
                              filesystem
                              log_setup
                              log
                              program_options
                              regex
                              serialization
                              system
                              thread
                              )

set(PROJECT_LIB ${PROJECT_LIB} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY})

set(mqtt_cpp_iface_DIR /usr/local/lib/cmake/mqtt_cpp_iface)
find_package(mqtt_cpp_iface REQUIRED)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
# add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(${PROJECT_NAME_STR} main.cc
configManager.cc
global.cc
gpioManager.cc
mqttWrapper.cc
oneWireSensorInterface.cc
powerControl.cc
powerControlStrategyExcess.cc
senecDataAcquisition.cc
senecResultObserver.cc
solarDataAcquisition.cc)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi PATHS /usr/local/include/wiringPi.h)
find_library(MOSQUITTO_LIBRARIES NAMES mosquittopp PATHS /usr/include/mosquittopp.h)

target_link_libraries(${PROJECT_NAME_STR} -Wl,--start-group  ${PROJECT_LIB} -Wl,--end-group)

target_link_libraries(${PROJECT_NAME_STR}
                      ${Boost_LIBRARIES}
                      ${WIRINGPI_LIBRARIES}
                      ${MOSQUITTO_LIBRARIES}
                      # ${Boost_LOG_LIBRARY}
                      mqtt_cpp_iface::mqtt_cpp_iface
                      stdc++fs)
                      # )