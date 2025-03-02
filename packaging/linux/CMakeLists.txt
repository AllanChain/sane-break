install(
  FILES ${CMAKE_SOURCE_DIR}/resources/images/icon.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
  RENAME io.github.AllanChain.sane-break.svg)
install(
  PROGRAMS sane-break.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
  RENAME io.github.AllanChain.sane-break.desktop)
install(
  FILES metainfo.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
  RENAME io.github.AllanChain.sane-break.metainfo.xml)
install(DIRECTORY gnome/sane-break@allanchain.github.io
        DESTINATION ${CMAKE_INSTALL_DATADIR}/gnome-shell/extensions)
