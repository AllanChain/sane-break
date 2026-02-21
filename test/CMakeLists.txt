find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(GTest)

enable_testing(true)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure)

include_directories("../src")

set(TEST_FILES test-app.cpp test-db.cpp test-window.cpp)
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

# Screenshot harness (not part of check target)
add_executable(
  test-screenshot test-screenshot.cpp ${CMAKE_SOURCE_DIR}/resources/index.qrc
)
target_link_libraries(test-screenshot PRIVATE Qt::Test sane-gui)

add_custom_target(
  screenshot
  COMMAND
    ${CMAKE_COMMAND} -E env "QT_QPA_PLATFORM=offscreen:size=1920x1080"
    "SCREENSHOT_DIR=${CMAKE_SOURCE_DIR}/screenshots" $<TARGET_FILE:test-screenshot>
  DEPENDS test-screenshot
)
