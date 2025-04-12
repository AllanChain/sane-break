find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(GTest)

enable_testing(true)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure)

include_directories("../src")

set(TEST_FILES test-app.cpp test-window.cpp)
set(UTIL_FILES dummy.h)

# app should use local settings
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/SaneBreak.ini)

foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_FILE_NAME "${TEST_FILE}" NAME_WLE)

  add_executable("${TEST_FILE_NAME}" "${TEST_FILE}" ${UTIL_FILES})
  target_link_libraries("${TEST_FILE_NAME}" PRIVATE Qt::Test GTest::gmock sane-core)
  add_test(NAME "${TEST_FILE_NAME}" COMMAND "${TEST_FILE_NAME}")

  add_dependencies(check "${TEST_FILE_NAME}")
endforeach()
