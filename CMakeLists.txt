cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(SDA VERSION 0.1.0 LANGUAGES CXX)
find_package(Boost REQUIRED COMPONENTS
                              filesystem 
                              regex 
                              serialization 
                              system
                              thread
                              unit_test_framework
            )

# add_executable(sda main.cc)
add_subdirectory(src)
# --- link against required libs ---
# target_link_libraries(serialization boost_serialization)

# testing
include(CTest)
enable_testing()
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
