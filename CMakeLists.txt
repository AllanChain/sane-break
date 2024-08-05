cmake_minimum_required(VERSION 3.5)

project(sane-break VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB PROJECT_SOURCES "src/*.cpp" "src/*.h" "resources/index.qrc")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB_RECURSE PROJECT_SOURCES_LINUX "src/linux/*")
    list(APPEND PROJECT_SOURCES ${PROJECT_SOURCES_LINUX})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
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
    find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
    find_package(Wayland REQUIRED COMPONENTS Client)
    # Files will generate after running build
    qt6_generate_wayland_protocol_client_sources(sane-break FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/linux/wayland/ext-idle-notify-v1.xml"
    )
    target_link_libraries(sane-break PRIVATE Qt::WaylandClient Wayland::Client)

    find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
    find_package(KF6 REQUIRED COMPONENTS IdleTime)
    target_link_libraries(sane-break PRIVATE KF6::IdleTime)
    add_subdirectory(packaging/linux/kde)
    find_package(LayerShellQt REQUIRED)
    if(LayerShellQt_FOUND)
        target_link_libraries(sane-break PUBLIC LayerShellQt::Interface)
    endif()
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

configure_file(src/config.h.in config.h)
include(GNUInstallDirs)
install(TARGETS sane-break
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(sane-break)
