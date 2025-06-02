find_package(Qt6 COMPONENTS Widgets WaylandClient)
find_package(Wayland COMPONENTS Client)
if(Qt6WaylandClient_FOUND AND WAYLAND_FOUND)
  qt_add_plugin(sane-wayland-check CLASS_NAME WaylandCheck)
  target_sources(sane-wayland-check PRIVATE check.cpp check.h)
  target_link_libraries(
    sane-wayland-check PRIVATE Qt6::Widgets Qt6::WaylandClient Wayland::Client
  )
  if(Qt6_VERSION_MINOR LESS 5) # Qt < 6.5 need private API to get wl_display
    target_link_libraries(sane-wayland-check PRIVATE Qt6::GuiPrivate)
  endif()
  set_target_properties(
    sane-wayland-check PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                                  OUTPUT_NAME "sanebreak_wayland_check"
  )
  install(TARGETS sane-wayland-check
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/sane-break
  )
endif()
