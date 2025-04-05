find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
qt_add_executable(
  sane-break-app MANUAL_FINALIZATION app.cpp app.h main.cpp
  ${CMAKE_SOURCE_DIR}/resources/index.qrc
)

find_package(Qt6 COMPONENTS LinguistTools)
if(Qt6_FOUND AND Qt6LinguistTools_FOUND)
  set(WITH_TRANSLATIONS
      ON
      CACHE BOOL "Enable translations"
  )
  file(GLOB TS_FILES "${CMAKE_SOURCE_DIR}/resources/translations/*.ts")
  set(OTHER_TS_FILES ${TS_FILES})
  list(FILTER OTHER_TS_FILES EXCLUDE REGEX ".*/(en|zh)\\.ts$")
  if(${Qt6_VERSION} VERSION_GREATER_EQUAL "6.7")
    message(STATUS "Enable modern translations")
    file(GLOB_RECURSE SOURCES_TO_TRANSLATE "${CMAKE_SOURCE_DIR}/src/*.cpp"
         "${CMAKE_SOURCE_DIR}/src/*.ui"
    )
    # zh.ts is used as template for new languages on Weblate. Therefore, any obsolute
    # strings should be removed
    qt_add_lupdate(
      TS_FILES
      "${CMAKE_SOURCE_DIR}/resources/translations/zh.ts"
      PLURALS_TS_FILE
      "${CMAKE_SOURCE_DIR}/resources/translations/en.ts"
      SOURCES
      ${SOURCES_TO_TRANSLATE}
      OPTIONS
      -no-obsolete
    )
    qt_add_lupdate(TS_FILES ${OTHER_TS_FILES} SOURCES ${SOURCES_TO_TRANSLATE})
    qt_add_lrelease(TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
    qt_add_resources(
      sane-break-app
      "translations"
      PREFIX
      "/i18n"
      BASE
      "${CMAKE_CURRENT_BINARY_DIR}"
      FILES
      "${QM_FILES}"
    )
  else()
    message(STATUS "Enable legacy translations")
    qt_add_translations(
      sane-break-app TS_FILES ${TS_FILES} LUPDATE_OPTIONS -no-obsolete
    )
  endif()
else()
  message(STATUS "Disable translations because LinguistTools is not found")
endif()

target_link_libraries(sane-break-app PRIVATE sane-gui)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(${CMAKE_SOURCE_DIR}/packaging/linux ${CMAKE_BINARY_DIR}/packaging)
  set(LINUX_DIST
      "Default"
      CACHE STRING "Linux distribution type"
  )
  set_property(CACHE LINUX_DIST PROPERTY STRINGS Flatpak Default)
  if(LINUX_DIST STREQUAL "Flatpak")
    message(STATUS "Building for Flatpak")
    set(LINUX_DIST_FLATPAK
        ON
        CACHE BOOL "Flatpak build"
    )
  elseif(LINUX_DIST STREQUAL "Default")
    unset(LINUX_DIST)
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(
    sane-break-app PRIVATE "${CMAKE_SOURCE_DIR}/packaging/windows/resources.rc"
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(MACOSX_BUNDLE_ICON_FILE icon.icns)
  set(SANE_BREAK_ICON ${CMAKE_SOURCE_DIR}/packaging/macos/icon.icns)
  set_source_files_properties(
    ${SANE_BREAK_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
  )
  target_sources(sane-break-app PRIVATE ${SANE_BREAK_ICON})

  set_target_properties(
    sane-break-app
    PROPERTIES MACOSX_BUNDLE_INFO_PLIST
               "${CMAKE_SOURCE_DIR}/packaging/macos/MacOSXBundleInfo.plist.in"
  )
endif()

set(MACOSX_BUNDLE_VERSION $ENV{GITHUB_RUN_NUMBER})
if(NOT DEFINED MACOSX_BUNDLE_VERSION)
  message(STATUS "Using fallback bundle version")
  set(MACOSX_BUNDLE_VERSION ${PROJECT_VERSION})
endif()
set_target_properties(
  sane-break-app
  PROPERTIES OUTPUT_NAME sane-break
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
             MACOSX_BUNDLE_BUNDLE_VERSION ${MACOSX_BUNDLE_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
             MACOSX_BUNDLE_GUI_IDENTIFIER io.github.AllanChain.sane-break
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE
             ${BUNDLE_ID_OPTION}
)

install(
  TARGETS sane-break-app
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

option(SANE_BUILD_DEB "Build Debian package" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND SANE_MAKE_DEB)
  message(STATUS "Making .deb packages")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_NAME "sane-break")
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
  set(CPACK_DEBIAN_FILE_NAME "sane-break.deb")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/AllanChain/sane-break/")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
      "A gentle break reminder that helps you avoid mindlessly skipping breaks"
  )
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Allan Chain")
  set(DEBIAN_DEPENDENCY_LIST "")
  if(ENABLE_WAYLAND)
    list(APPEND DEBIAN_DEPENDENCY_LIST "qt6-wayland(>=6.6)")
  endif()
  if(ENABLE_X11)
    list(APPEND DEBIAN_DEPENDENCY_LIST "libxss1(>=1.2.3)")
  endif()
  if(WITH_LAYER_SHELL)
    list(APPEND DEBIAN_DEPENDENCY_LIST "layer-shell-qt(>=6.2)")
  endif()
  list(JOIN DEBIAN_DEPENDENCY_LIST ", " CPACK_DEBIAN_PACKAGE_DEPENDS)
  include(CPack)
endif()

qt_finalize_executable(sane-break-app)
