install(
  FILES ${CMAKE_SOURCE_DIR}/resources/images/icon.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
  RENAME io.github.AllanChain.sane-break.svg
)
install(
  PROGRAMS sane-break.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
  RENAME io.github.AllanChain.sane-break.desktop
)
install(
  FILES metainfo.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
  RENAME io.github.AllanChain.sane-break.metainfo.xml
)
install(DIRECTORY gnome/sane-break@allanchain.github.io
        DESTINATION ${CMAKE_INSTALL_DATADIR}/gnome-shell/extensions
)

option(SANE_BUILD_DEB "Build Debian package" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND SANE_MAKE_DEB)
  message(STATUS "Making .deb packages")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_NAME "sane-break")
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
  set(CPACK_DEBIAN_FILE_NAME "sane-break.deb")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/AllanChain/sane-break/")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
      "A gentle break reminder that helps you avoid mindlessly skipping breaks"
  )
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Allan Chain")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6gui6(>=6.4), libqt6multimedia6(>=6.4)")
  set(CPACK_DEBIAN_PACKAGE_RECOMMENDS
      "qt6-wayland(>=6.4), layer-shell-qt(>=6.2), libxss1(>=1.2.3)"
  )
  include(CPack)
endif()
