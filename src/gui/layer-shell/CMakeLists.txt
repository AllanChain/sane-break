find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(LayerShellQt)
if(LayerShellQt_FOUND)
  message(STATUS "Enable LayerShell support")
  qt_add_plugin(sane-layer-shell CLASS_NAME LayerShell)
  target_sources(sane-layer-shell PRIVATE layer-shell.cpp layer-shell.h)
  target_link_libraries(sane-layer-shell PRIVATE Qt6::Widgets LayerShellQt::Interface)
  set_target_properties(
    sane-layer-shell PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                                OUTPUT_NAME "sanebreak_layer_shell"
  )
  install(TARGETS sane-layer-shell
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/sane-break
  )
endif()
