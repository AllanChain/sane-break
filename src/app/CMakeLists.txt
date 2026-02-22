find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Sql)

add_library(
  sane-gui STATIC
  app.cpp
  app.h
  break-window.cpp
  break-window.h
  break-window.ui
  break-windows.cpp
  break-windows.h
  meeting-prompt.cpp
  meeting-prompt.h
  meeting-prompt.ui
  focus-window.cpp
  focus-window.h
  focus-window.ui
  meeting-window.cpp
  meeting-window.h
  meeting-window.ui
  pref-window.cpp
  pref-window.h
  pref-window.ui
  postpone-window.cpp
  postpone-window.h
  postpone-window.ui
  sound-player.cpp
  sound-player.h
  stats-window.cpp
  stats-window.h
  stats-window.ui
  text-window.cpp
  text-window.h
  tray.cpp
  tray.h
  welcome.cpp
  welcome.h
  widgets/timeline-view.cpp
  widgets/timeline-view.h
  widgets/color-edit.h
  widgets/dynamic-button.h
  widgets/dynamic-label.h
  widgets/stepped-slider.cpp
  widgets/stepped-slider.h
  widgets/language-select.cpp
  widgets/language-select.h
  widgets/wrapped-checkbox.cpp
  widgets/wrapped-checkbox.h
)

target_include_directories(
  sane-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
)
target_link_libraries(sane-gui PUBLIC Qt6::Widgets Qt6::Multimedia sane-lib)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_sources(sane-gui PRIVATE unix/signal-handler.h unix/signal-handler.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(sane-gui PRIVATE windows/signal-handler.h windows/signal-handler.cpp)
endif()

qt_add_executable(
  sane-break-app MANUAL_FINALIZATION main.cpp ${CMAKE_SOURCE_DIR}/resources/index.qrc
)
target_link_libraries(sane-break-app PRIVATE sane-gui)

include(cmake/translation.cmake)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(layer-shell)
  add_subdirectory(${CMAKE_SOURCE_DIR}/packaging/linux ${CMAKE_BINARY_DIR}/packaging)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_subdirectory(${CMAKE_SOURCE_DIR}/packaging/windows ${CMAKE_BINARY_DIR}/packaging)
  target_sources(
    sane-break-app PRIVATE "${CMAKE_SOURCE_DIR}/packaging/windows/resources.rc"
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(SANE_BREAK_ICON_LEGACY ${CMAKE_SOURCE_DIR}/packaging/macos/icon.icns)
  set(SANE_BREAK_ICON_TAHOE ${CMAKE_SOURCE_DIR}/packaging/macos/Assets.car)
  target_sources(sane-break-app PRIVATE ${SANE_BREAK_ICON} ${SANE_BREAK_ICON_TAHOE})
  set_source_files_properties(
    ${SANE_BREAK_ICON_LEGACY} ${SANE_BREAK_ICON_TAHOE}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
  )

  set(MACOSX_BUNDLE_VERSION $ENV{GITHUB_RUN_NUMBER})
  if(NOT DEFINED MACOSX_BUNDLE_VERSION)
    message(STATUS "Using fallback bundle version")
    set(MACOSX_BUNDLE_VERSION ${PROJECT_VERSION})
  endif()

  set_target_properties(
    sane-break-app
    PROPERTIES MACOSX_BUNDLE_INFO_PLIST
               "${CMAKE_SOURCE_DIR}/packaging/macos/MacOSXBundleInfo.plist.in"
               MACOSX_BUNDLE_BUNDLE_VERSION ${MACOSX_BUNDLE_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
               MACOSX_BUNDLE_GUI_IDENTIFIER io.github.AllanChain.sane-break
               MACOSX_BUNDLE TRUE
  )
endif()

set_target_properties(
  sane-break-app
  PROPERTIES OUTPUT_NAME sane-break
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
             WIN32_EXECUTABLE TRUE
)

install(
  TARGETS sane-break-app
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(sane-break-app)
