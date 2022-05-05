cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
set(PROJECT_NAME_STR sda)
project(${PROJECT_NAME_STR} VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
file(GLOB_RECURSE INC_FILES src/*.hh)
file(GLOB_RECURSE SRC_FILES src/*.cc)

# find_package(wiringpi)

# add_executable(sda main.cc)
add_subdirectory(src)
# --- link against required libs ---
# target_link_libraries(serialization boost_serialization)

# testing
include(CTest)
add_subdirectory(test)

include_directories(${Boost_INCLUDE_DIRS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
