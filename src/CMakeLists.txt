find_package(Boost REQUIRED COMPONENTS
                              filesystem 
                              regex 
                              serialization 
                              system
                              thread
            )

add_executable(${PROJECT_NAME_STR} main.cc senecDataAcquisition.cc solarDataAcquisition.cc)
target_link_libraries(${PROJECT_NAME_STR} ${Boost_LIBRARIES})