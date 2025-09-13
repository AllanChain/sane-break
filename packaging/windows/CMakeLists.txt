option(SANE_BUILD_NSIS "Build NSIS package" OFF)
message(STATUS "Making NSIS packages")
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "sane-break")
set(CPACK_PACKAGE_EXECUTABLES "sane-break" "Sane Break")
set(CPACK_PACKAGE_FILE_NAME "sane-break-windows")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "A gentle break reminder that helps you avoid mindlessly skipping breaks"
)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_NSIS_DISPLAY_NAME "Sane Break")
set(CPACK_NSIS_PACKAGE_NAME "Sane Break")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
set(CPACK_NSIS_MUI_UNOICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/sane-break.exe")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "sane-break")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/AllanChain/sane-break/")
set(CPACK_NSIS_CONTACT "https://github.com/AllanChain/sane-break/issues/")
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS
    [[
  nsExec::Exec 'cmd /c tasklist /fi \"imagename eq sane-break.exe\" /nh > \"$TEMP\\sanebreak_check.txt\"'
  FileOpen $0 \"$TEMP\\sanebreak_check.txt\" r
  FileRead $0 $1
  FileClose $0
  Delete \"$TEMP\\sanebreak_check.txt\"

  StrCpy $2 \"INFO: No tasks are running which match the specified criteria.\"
  \${If} $1 S!= \"$2$\\r$\\n\"
  \${AndIf} $1 S!= \"$2$\\n\"
  MessageBox MB_YESNO|MB_ICONQUESTION \"Sane Break is currently running.$\\n$\\nDo you want to terminate it to continue the installation?\" IDYES terminate IDNO abort
      terminate:
          nsExec::Exec 'taskkill /IM sane-break.exe /F'
          Sleep 1000 ; Wait a second to ensure the process is terminated
      Goto continue
      abort:
          Abort \"Installation aborted by user.\"
      continue:
  \${EndIf}
]]
)
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "nsExec::Exec 'taskkill /IM sane-break.exe /F'")
include(CPack)
