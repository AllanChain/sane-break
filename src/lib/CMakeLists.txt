add_library(
  sane-lib STATIC
  auto-start.h
  app-core.cpp
  app-core.h
  battery-status.cpp
  battery-status.h
  CMakeLists.txt
  idle-time.cpp
  idle-time.h
  preferences.cpp
  preferences.h
  program-monitor.cpp
  program-monitor.h
  screen-lock.cpp
  screen-lock.h
  system-monitor.cpp
  system-monitor.h
  utils.cpp
  utils.h
)
target_include_directories(sane-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(
  Qt6
  COMPONENTS Widgets
  REQUIRED
)
target_link_libraries(sane-lib PUBLIC Qt6::Widgets)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_library(IOKit NAMES IOKit)
  find_library(CoreFoundation NAMES CoreFoundation)
  find_library(ServiceManagement NAMES ServiceManagement)
  target_link_libraries(sane-lib PRIVATE "-framework IOKit")
  target_link_libraries(sane-lib PRIVATE "-framework CoreFoundation")
  target_link_libraries(sane-lib PRIVATE "-framework ServiceManagement")
  target_sources(
    sane-lib
    PRIVATE macos/auto-start.mm
            macos/battery.cpp
            macos/battery.h
            macos/idle.cpp
            macos/idle.h
            macos/workspace.h
            macos/workspace.mm
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(
    sane-lib PRIVATE windows/auto-start.cpp windows/battery.cpp windows/battery.h
                     windows/idle.cpp windows/idle.h
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(
    Qt6
    COMPONENTS DBus
    REQUIRED
  )
  target_link_libraries(sane-lib PRIVATE Qt6::DBus)

  option(AUTO_ENABLE_X11 "Build with X11 support" ON)
  option(AUTO_ENABLE_WAYLAND "Build with Wayland support" ON)

  target_sources(
    sane-lib PRIVATE linux/auto-start.cpp linux/battery.cpp linux/battery.h
                     linux/system-check.cpp linux/system-check.h
  )

  if(AUTO_ENABLE_X11)
    include(FindPkgConfig)
    pkg_check_modules(X11 x11)
    pkg_check_modules(XSCRNSAVER xscrnsaver)
    if(X11_FOUND AND XSCRNSAVER_FOUND)
      set(ENABLE_X11
          ON
          CACHE BOOL "Enable X11"
      )
      message(STATUS "Enable X11 support")
      target_sources(sane-lib PRIVATE linux/x11/idle.cpp linux/x11/idle.h)
      target_link_libraries(sane-lib PRIVATE ${X11_LIBRARIES})
      target_link_libraries(sane-lib PRIVATE ${XSCRNSAVER_LIBRARIES})
    else()
      set(ENABLE_X11
          OFF
          CACHE BOOL "Enable X11"
      )
      message(STATUS "X11 support not enabled due to missing dependencies:")
      if(NOT X11_FOUND)
        message(STATUS "  X11 (missing)")
      endif()
      if(NOT XSCRNSAVER_FOUND)
        message(STATUS "  XSCRNSAVER (missing)")
      endif()
    endif()
  else()
    set(ENABLE_X11
        OFF
        CACHE BOOL "Enable X11"
    )
    message(STATUS "X11 support is disabled")
  endif()
  if(ENABLE_X11)

  endif()

  if(AUTO_ENABLE_WAYLAND)
    find_package(Qt6 COMPONENTS WaylandClient)
    find_package(Wayland COMPONENTS Client)
    find_package(LayerShellQt)
    if(Qt6WaylandClient_FOUND AND WAYLAND_FOUND)
      set(ENABLE_WAYLAND
          ON
          CACHE BOOL "Enable Wayland"
      )
      message(STATUS "Enable Wayland support")
      target_sources(sane-lib PRIVATE linux/wayland/idle.cpp linux/wayland/idle.h)
      # Files will generate after running build
      qt6_generate_wayland_protocol_client_sources(
        sane-lib FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/linux/wayland/ext-idle-notify-v1.xml"
      )
      target_link_libraries(sane-lib PRIVATE Wayland::Client)
      if(LayerShellQt_FOUND)
        set(WITH_LAYER_SHELL
            ON
            CACHE BOOL "Compile with layer shell"
        )
        target_link_libraries(sane-lib PUBLIC LayerShellQt::Interface)
      endif()
    else()
      set(ENABLE_WAYLAND
          OFF
          CACHE BOOL "Enable Wayland"
      )
      message(STATUS "Wayland support not enabled due to missing dependencies:")
      if(NOT WAYLAND_FOUND)
        message(STATUS "  Wayland Client (missing)")
      endif()
      if(NOT Qt6WaylandClient_FOUND)
        message(STATUS "  Qt6 Wayland (missing)")
      endif()
      if(NOT WITH_LAYER_SHELL)
        message(STATUS "  Layer Shell Qt (missing)")
      endif()
    endif()
  else()
    set(ENABLE_WAYLAND
        OFF
        CACHE BOOL "Enable Wayland"
    )
    message(STATUS "Wayland support is disabled")
  endif()

  if(NOT ENABLE_X11 AND NOT ENABLE_WAYLAND)
    message(FATAL_ERROR "Please install X11 or Wayland libraries")
  endif()
endif()
