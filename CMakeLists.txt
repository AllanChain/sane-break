cmake_minimum_required(VERSION 3.5)

project(sane-break VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.h" "src/*.mm" "resources.qrc")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(SANE_BREAK_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/icon.icns)
    set_source_files_properties(${SANE_BREAK_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND PROJECT_SOURCES ${SANE_BREAK_ICON})
endif()

qt_add_executable(sane-break
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(sane-break PRIVATE "src")
target_link_libraries(sane-break PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
    find_package(KF6 REQUIRED COMPONENTS IdleTime)
    find_package(LayerShellQt REQUIRED)
    target_link_libraries(sane-break PRIVATE KF6::IdleTime)
    target_link_libraries(sane-break PUBLIC LayerShellQt::Interface)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(IOKit NAMES IOKit)
    find_library(CoreFoundation NAMES CoreFoundation)
    target_link_libraries(sane-break PRIVATE "-framework IOKit")
    target_link_libraries(sane-break PRIVATE "-framework CoreFoundation")
    set_target_properties(sane-break PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in")
endif()

set_target_properties(sane-break PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS sane-break
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(sane-break)
