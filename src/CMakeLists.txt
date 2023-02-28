find_package(Boost REQUIRED COMPONENTS
                              filesystem
                              log
                              log_setup
                              regex
                              serialization
                              system
                              thread
                              )
set(mqtt_cpp_iface_DIR )
# find_package(mqtt_cpp_iface REQUIRED)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

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

target_link_libraries(${PROJECT_NAME_STR}
                      ${Boost_LIBRARIES}
                      ${WIRINGPI_LIBRARIES}
                      ${MOSQUITTO_LIBRARIES}
                      )