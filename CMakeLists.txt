cmake_minimum_required(VERSION 3.10)

# Should be set before project call and be a cache variable
set(CMAKE_OSX_DEPLOYMENT_TARGET
    "13"
    CACHE STRING "Minimum OS X deployment version"
)

project(
  sane-break
  VERSION 0.7.0
  LANGUAGES C CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

include(GNUInstallDirs)

configure_file(config.h.cmake config.h)

add_subdirectory(src)
