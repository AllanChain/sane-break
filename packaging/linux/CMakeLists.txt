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

