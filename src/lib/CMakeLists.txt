add_library(
  sane-lib STATIC
  auto-start.h
  battery-status.cpp
  battery-status.h
  program-monitor.cpp
  program-monitor.h
  screen-lock.h
  screen-monitor.cpp
  screen-monitor.h
  sleep-monitor.cpp
  sleep-monitor.h
  system-monitor.cpp
  system-monitor.h
  timer.h
  utils.cpp
  utils.h
)
target_include_directories(sane-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(
  Qt6
  COMPONENTS Widgets Sql
  REQUIRED
)
target_link_libraries(sane-lib PUBLIC Qt6::Widgets sane-core sane-idle)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_link_libraries(sane-lib PRIVATE "-framework IOKit")
  target_link_libraries(sane-lib PRIVATE "-framework CoreFoundation")
  target_link_libraries(sane-lib PRIVATE "-framework ServiceManagement")
  target_sources(
    sane-lib
    PRIVATE macos/auto-start.mm
            macos/battery.cpp
            macos/battery.h
            macos/program-monitor.mm
            macos/screen-lock.cpp
            macos/workspace.h
            macos/workspace.mm
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(
    sane-lib PRIVATE windows/auto-start.cpp windows/battery.cpp windows/battery.h
                     windows/screen-lock.cpp windows/program-monitor.cpp
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(
    Qt6
    COMPONENTS DBus
    REQUIRED
  )
  target_link_libraries(sane-lib PRIVATE Qt6::DBus)
  target_sources(
    sane-lib
    PRIVATE linux/auto-start.cpp
            linux/battery.cpp
            linux/battery.h
            linux/screen-lock.cpp
            linux/program-monitor.cpp
            linux/system-check.cpp
            linux/system-check.h
  )
  add_subdirectory(linux/wayland-check)
endif()
