find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_library(
  sane-core STATIC
  app.cpp
  app.h
  flags.h
  idle-time.h
  preferences.cpp
  preferences.h
  system-monitor.h
  timer.h
  window-control.cpp
  window-control.h
)
target_include_directories(sane-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sane-core PUBLIC Qt6::Widgets)
