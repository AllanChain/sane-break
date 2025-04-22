find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
add_library(
  sane-gui STATIC
  break-window.cpp
  break-window.h
  pref-window.cpp
  pref-window.h
  pref-window.ui
  sound-player.cpp
  sound-player.h
  tray.cpp
  tray.h
  welcome.cpp
  welcome.h
  widgets/color-edit.h
  widgets/dynamic-label.h
  widgets/stepped-slider.cpp
  widgets/stepped-slider.h
  widgets/language-select.cpp
  widgets/language-select.h
  break-window.ui
)
target_include_directories(sane-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sane-gui PUBLIC Qt6::Widgets Qt6::Multimedia sane-lib)
