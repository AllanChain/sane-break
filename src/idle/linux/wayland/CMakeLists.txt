find_package(Qt6 COMPONENTS Widgets WaylandClient)
find_package(Wayland COMPONENTS Client)
if(Qt6WaylandClient_FOUND AND WAYLAND_FOUND)
  qt_add_plugin(sane-idle-wayland CLASS_NAME IdleTimeWayland)
  target_sources(sane-idle-wayland PRIVATE wayland-idle.cpp wayland-idle.h)
  qt6_generate_wayland_protocol_client_sources(
    sane-idle-wayland FILES "${CMAKE_CURRENT_SOURCE_DIR}/ext-idle-notify-v1.xml"
  )
  target_link_libraries(
    sane-idle-wayland PRIVATE Qt6::Widgets Wayland::Client sane-core-idle
  )
  if(Qt6_VERSION_MINOR LESS 5) # Qt < 6.5 need private API to get wl_display
    target_link_libraries(sane-idle-wayland PRIVATE Qt6::GuiPrivate)
  endif()
  set_target_properties(
    sane-idle-wayland PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                                 OUTPUT_NAME "sanebreak_idle_wayland"
  )
  install(TARGETS sane-idle-wayland
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/sane-break
  )
endif()
