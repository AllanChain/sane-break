find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
qt_add_plugin(sane-idle-gnome CLASS_NAME IdleTimeGNOME)
target_sources(sane-idle-gnome PRIVATE gnome-idle.cpp gnome-idle.h)
target_link_libraries(sane-idle-gnome PRIVATE Qt6::Widgets Qt6::DBus)
set_target_properties(
  sane-idle-gnome PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                             OUTPUT_NAME "sanebreak_idle_gnome"
)
install(TARGETS sane-idle-gnome LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/sane-break)
