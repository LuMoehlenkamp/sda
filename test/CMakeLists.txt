# --- build test executables ---
add_executable(MainTest mainTest.cc)
target_link_libraries(
  MainTest
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
add_test(MainTest MainTest)