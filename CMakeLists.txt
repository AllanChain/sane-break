cmake_minimum_required(VERSION 3.12)

# Should be set before project call and be a cache variable
set(CMAKE_OSX_DEPLOYMENT_TARGET
    "13"
    CACHE STRING "Minimum OS X deployment version"
)

project(
  sane-break
  VERSION 0.9.4
  LANGUAGES C CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)

add_subdirectory(src)

option(TESTING "Build with testing" OFF)
if(TESTING)
  add_subdirectory(test)
endif()

configure_file(config.h.cmake config.h)
