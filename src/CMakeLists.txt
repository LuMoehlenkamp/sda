find_package(Boost REQUIRED COMPONENTS
                              filesystem 
                              regex 
                              serialization 
                              system
                              thread
            )
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
find_library(wiringPi_LIB wiringPi)

add_executable(${PROJECT_NAME_STR} main.cc senecDataAcquisition.cc solarDataAcquisition.cc)
target_link_libraries(${PROJECT_NAME_STR} ${Boost_LIBRARIES} ${wiringPi_LIB})