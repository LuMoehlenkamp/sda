add_executable(sda main.cc senecDataAcquisition.cc)
target_link_libraries(sda ${Boost_LIBRARIES})