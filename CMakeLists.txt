cmake_minimum_required(VERSION 3.10)

# Should be set before project call and be a cache variable
set(CMAKE_OSX_DEPLOYMENT_TARGET
    "12"
    CACHE STRING "Minimum OS X deployment version")

project(
  sane-break
  VERSION 0.6.2
  LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_add_executable(sane-break MANUAL_FINALIZATION src/app.cpp src/app.h
                  src/main.cpp resources/index.qrc)
add_subdirectory(src/lib)
add_subdirectory(src/gui)

find_package(Qt6 COMPONENTS LinguistTools)
if(Qt6_FOUND AND Qt6LinguistTools_FOUND)
  set(WITH_TRANSLATIONS ON)
  message(STATUS "Enable translations")
  file(GLOB TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/translations/*.ts")
  qt_add_translations(sane-break TS_FILES ${TS_FILES})
else()
  message(STATUS "Disable translations because LinguistTools is not found")
endif()

target_include_directories(sane-break PRIVATE "src" ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sane-break PRIVATE Qt6::Widgets Qt6::Multimedia sane-lib
                                         sane-gui)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(packaging/linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(
    sane-break
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/resources.rc")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(MACOSX_BUNDLE_ICON_FILE icon.icns)
  set(SANE_BREAK_ICON ${CMAKE_CURRENT_SOURCE_DIR}/packaging/macos/icon.icns)
  set_source_files_properties(${SANE_BREAK_ICON}
                              PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  target_sources(sane-break PRIVATE ${SANE_BREAK_ICON})

  find_library(IOKit NAMES IOKit)
  find_library(CoreFoundation NAMES CoreFoundation)
  target_link_libraries(sane-break PRIVATE "-framework IOKit")
  target_link_libraries(sane-break PRIVATE "-framework CoreFoundation")
  set_target_properties(
    sane-break
    PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST
      "${CMAKE_CURRENT_SOURCE_DIR}/packaging/macos/MacOSXBundleInfo.plist.in")
endif()

set(MACOSX_BUNDLE_VERSION $ENV{GITHUB_RUN_NUMBER})
if(NOT DEFINED MACOSX_BUNDLE_VERSION)
  message(STATUS "Using fallback bundle version")
  set(MACOSX_BUNDLE_VERSION ${PROJECT_VERSION})
endif()
set_target_properties(
  sane-break
  PROPERTIES ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
             ${MACOSX_BUNDLE_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION} MACOSX_BUNDLE_GUI_IDENTIFIER
             io.github.AllanChain.sane-break MACOSX_BUNDLE
             TRUE WIN32_EXECUTABLE
             TRUE)

configure_file(src/config.h.cmake config.h)
install(
  TARGETS sane-break
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

option(SANE_BUILD_DEB "Build Debian package" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND SANE_MAKE_DEB)
  message(STATUS "Making .deb packages")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_NAME "sane-break")
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
  set(CPACK_DEBIAN_FILE_NAME "sane-break.deb")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/AllanChain/sane-break/")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
      "A gentle break reminder that helps you avoid mindlessly skipping breaks")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Allan Chain")
  set(DEBIAN_DEPENDENCY_LIST "")
  if(ENABLE_WAYLAND)
    list(APPEND DEBIAN_DEPENDENCY_LIST "qt6-wayland(>=6.6)")
  endif()
  if(ENABLE_X11)
    list(APPEND DEBIAN_DEPENDENCY_LIST "libxss1(>=1.2.3)")
  endif()
  if(LayerShellQt_FOUND)
    list(APPEND DEBIAN_DEPENDENCY_LIST "layer-shell-qt(>=6.2)")
  endif()
  list(JOIN DEBIAN_DEPENDENCY_LIST ", " CPACK_DEBIAN_PACKAGE_DEPENDS)
  include(CPack)
endif()

qt_finalize_executable(sane-break)
