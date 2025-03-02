add_library(sane-gui STATIC
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
  widgets/stepped-slider.cpp
  widgets/stepped-slider.h
  widgets/language-select.cpp
  widgets/language-select.h
  window-manager.cpp
  window-manager.h
)
target_include_directories(sane-gui
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}
)
target_link_libraries(sane-gui PRIVATE Qt6::Widgets Qt6::Multimedia sane-lib)
