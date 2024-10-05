cmake_minimum_required(VERSION 3.5)

# Should be set before project call
set(CMAKE_OSX_DEPLOYMENT_TARGET "12")

project(sane-break VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

include(GNUInstallDirs)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB PROJECT_SOURCES "src/*.cpp" "src/*.h" "resources/index.qrc")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB_RECURSE PROJECT_SOURCES_MACOS "src/macos/*")
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(SANE_BREAK_ICON ${CMAKE_CURRENT_SOURCE_DIR}/packaging/macos/icon.icns)
    set_source_files_properties(${SANE_BREAK_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND PROJECT_SOURCES ${PROJECT_SOURCES_MACOS} ${SANE_BREAK_ICON})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB_RECURSE PROJECT_SOURCES_WINDOWS "src/windows/*")
    set(RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/resources.rc")
    list(APPEND PROJECT_SOURCES ${PROJECT_SOURCES_WINDOWS} ${RESOURCE_WINDOWS})
endif()

qt_add_executable(sane-break
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(sane-break PRIVATE "src" ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sane-break PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB PROJECT_SOURCES_LINUX "src/linux/*")
    target_sources(sane-break PRIVATE ${PROJECT_SOURCES_LINUX})

    include(FindPkgConfig)
    pkg_check_modules(X11 x11)
    pkg_check_modules(XSCRNSAVER xscrnsaver)
    if(X11_FOUND AND XSCRNSAVER_FOUND)
        set(ENABLE_X11 ON)
        message(STATUS "Enable X11 support")
    else()
        message(STATUS "X11 support not enabled due to missing dependencies:")
        if(NOT X11_FOUND)
            message(STATUS "  X11 (missing)")
        endif()
        if(NOT XSCRNSAVER_FOUND)
            message(STATUS "  XSCRNSAVER (missing)")
        endif()
    endif()
    if(DEFINED ENABLE_X11)
        file(GLOB PROJECT_SOURCES_LINUX "src/linux/x11/*")
        target_sources(sane-break PRIVATE ${PROJECT_SOURCES_LINUX})
        target_link_libraries(sane-break PRIVATE ${X11_LIBRARIES})
        target_link_libraries(sane-break PRIVATE ${XSCRNSAVER_LIBRARIES})
    endif()

    find_package(Qt6 COMPONENTS WaylandClient)
    find_package(Wayland COMPONENTS Client)
    find_package(LayerShellQt)
    if(Qt6_FOUND AND WAYLAND_FOUND AND LayerShellQt_FOUND)
        set(ENABLE_WAYLAND ON)
        message(STATUS "Enable Wayland support")
    else()
        message(STATUS "Wayland support not enabled due to missing dependencies:")
        if (NOT WAYLAND_FOUND)
            message(STATUS "  Wayland Client (missing)")
        endif()
        if(NOT Qt6_FOUND)
            message(STATUS "  Qt6 Wayland (missing)")
        endif()
        if(NOT LayerShellQt_FOUND)
            message(STATUS "  Layer Shell Qt (missing)")
        endif()
    endif()
    if(DEFINED ENABLE_WAYLAND)
        file(GLOB PROJECT_SOURCES_LINUX "src/linux/wayland/*")
        target_sources(sane-break PRIVATE ${PROJECT_SOURCES_LINUX})
        # Files will generate after running build
        qt6_generate_wayland_protocol_client_sources(sane-break FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/linux/wayland/ext-idle-notify-v1.xml"
        )
        target_link_libraries(sane-break PRIVATE Qt::WaylandClient Wayland::Client LayerShellQt::Interface)
    endif()

    if(NOT ENABLE_X11 AND NOT ENABLE_WAYLAND)
        message(FATAL_ERROR "Please install X11 or Wayland libraries")
    endif()

    find_package(Qt6 COMPONENTS DBus REQUIRED)
    target_link_libraries(sane-break PRIVATE Qt::DBus)

    add_subdirectory(packaging/linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(IOKit NAMES IOKit)
    find_library(CoreFoundation NAMES CoreFoundation)
    target_link_libraries(sane-break PRIVATE "-framework IOKit")
    target_link_libraries(sane-break PRIVATE "-framework CoreFoundation")
    set_target_properties(sane-break PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/macos/MacOSXBundleInfo.plist.in")
endif()

set_target_properties(sane-break PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

configure_file(src/config.h.cmake config.h)
install(TARGETS sane-break
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(sane-break)
