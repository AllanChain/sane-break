find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_library(
  sane-idle STATIC
  factory.cpp
  factory.h
  idle-interface.h
  idle-reader.cpp
  idle-reader.h
  read-based-idle.cpp
  read-based-idle.h
)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Qt6 REQUIRED COMPONENTS DBus)
  target_sources(
    sane-idle
    PRIVATE linux/factory.h linux/factory.cpp linux/inhibitor-proxy-idle.h
            linux/inhibitor-proxy-idle.cpp linux/linux-idle-inhibitor.h
            linux/linux-idle-inhibitor.cpp
  )
  target_link_libraries(sane-idle PRIVATE Qt6::DBus)
  add_subdirectory(linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(
    sane-idle PRIVATE windows-idle.h windows-inhibitor.h windows-inhibitor.cpp
  )
  target_link_libraries(sane-idle PRIVATE PowrProf)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_sources(sane-idle PRIVATE macos-idle.h macos-inhibitor.h macos-inhibitor.cpp)
endif()
target_link_libraries(sane-idle PUBLIC Qt6::Widgets)
