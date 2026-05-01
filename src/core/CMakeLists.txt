find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)

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

set(SANE_BREAK_CLI_CONTROL_ENABLED OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(CLI11 CONFIG QUIET)
  if(CLI11_FOUND)
    set(SANE_BREAK_CLI_CONTROL_ENABLED ON)
  else()
    message(WARNING "CLI11 not found; building without CLI control feature.")
  endif()
else()
  include(FetchContent)
  FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.6.2
  )
  FetchContent_MakeAvailable(cli11_proj)
  set(SANE_BREAK_CLI_CONTROL_ENABLED ON)
endif()

set(SANE_BREAK_CLI_CONTROL_ENABLED
    ${SANE_BREAK_CLI_CONTROL_ENABLED}
    CACHE INTERNAL "Whether CLI control feature is built" FORCE
)

target_link_libraries(
  sane-core PUBLIC Qt6::Widgets Qt6::Sql Qt6::Network sane-core-idle
)

if(SANE_BREAK_CLI_CONTROL_ENABLED)
  target_sources(sane-core PRIVATE cli.cpp cli.h command-ipc.cpp command-ipc.h)
  target_link_libraries(sane-core PRIVATE CLI11::CLI11)
endif()
