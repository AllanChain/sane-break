find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# core/idle-time.h is also used by sane-idle, so we are making it separate
add_library(sane-core-idle STATIC idle-time.h)
set_target_properties(sane-core-idle PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(sane-core-idle PRIVATE BUILD_COREIDLE_LIB)
target_link_libraries(sane-core-idle PRIVATE Qt6::Widgets)

add_library(
  sane-core STATIC
  app-data.cpp
  app-data.h
  app-states.cpp
  app-states.h
  app.cpp
  app.h
  break-windows.h
  meeting-prompt.h
  flags.h
  db.h
  db.cpp
  preferences.cpp
  preferences.h
  system-monitor.h
  timer.h
)
target_link_libraries(sane-core PUBLIC Qt6::Widgets Qt6::Sql sane-core-idle)
