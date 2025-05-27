find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
add_library(
  sane-gui STATIC
  break-window.cpp
  break-window.h
  break-window.ui
  pref-window.cpp
  pref-window.h
  pref-window.ui
  sound-player.cpp
  sound-player.h
  text-window.cpp
  text-window.h
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
  widgets/wrapped-checkbox.cpp
  widgets/wrapped-checkbox.h
)
target_include_directories(sane-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sane-gui PUBLIC Qt6::Widgets Qt6::Multimedia sane-lib)
