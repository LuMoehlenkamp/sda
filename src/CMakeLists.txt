find_package(Boost REQUIRED COMPONENTS
                              filesystem
                              log
                              log_setup
                              regex
                              serialization
                              system
                              thread
            )
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

add_executable(${PROJECT_NAME_STR} main.cc
configManager.cc
gpioManager.cc
powerControl.cc
senecDataAcquisition.cc
senecResultObserver.cc
solarDataAcquisition.cc
global.cc)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi PATHS /usr/local/include/wiringPi.h)

target_link_libraries(${PROJECT_NAME_STR}
                      ${Boost_LIBRARIES}
                      ${WIRINGPI_LIBRARIES}
                      )