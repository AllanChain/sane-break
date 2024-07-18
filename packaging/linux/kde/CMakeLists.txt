include(ECMInstallIcons)
include(KDEInstallDirs)

ecm_install_icons(ICONS 256-apps-sane-break.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)
install(PROGRAMS ../sane-break.desktop DESTINATION ${KDE_INSTALL_APPDIR})
