find_package(Qt6 REQUIRED COMPONENTS Widgets)
include(FindPkgConfig)
pkg_check_modules(X11 x11)
pkg_check_modules(XSCRNSAVER xscrnsaver)
if(X11_FOUND AND XSCRNSAVER_FOUND)
  qt_add_plugin(sane-idle-x11 CLASS_NAME IdleTimeX11)
  target_sources(sane-idle-x11 PRIVATE x11-idle.cpp x11-idle.h)
  target_link_libraries(
    sane-idle-x11 PRIVATE Qt6::Widgets ${X11_LIBRARIES} ${XSCRNSAVER_LIBRARIES}
  )
  target_include_directories(sane-idle-x11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  set_target_properties(
    sane-idle-x11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                             OUTPUT_NAME "sanebreak_idle_x11"
  )
  install(TARGETS sane-idle-x11 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/sane-break)
endif()
