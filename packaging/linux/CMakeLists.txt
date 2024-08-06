install(
  FILES ${CMAKE_SOURCE_DIR}/resources/images/icon.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
  RENAME sane-break.svg
)
install(PROGRAMS sane-break.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

