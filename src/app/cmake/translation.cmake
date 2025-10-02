find_package(Qt6 COMPONENTS LinguistTools)
if(Qt6_FOUND AND Qt6LinguistTools_FOUND)
  set(WITH_TRANSLATIONS
      ON
      CACHE BOOL "Enable translations"
  )
  file(GLOB TS_FILES "${CMAKE_SOURCE_DIR}/resources/translations/*.ts")
  set(OTHER_TS_FILES ${TS_FILES})
  list(FILTER OTHER_TS_FILES EXCLUDE REGEX ".*/(en|zh_CN)\\.ts$")
  if(${Qt6_VERSION} VERSION_GREATER_EQUAL "6.7")
    message(STATUS "Enable modern translations")
    file(GLOB_RECURSE SOURCES_TO_TRANSLATE "${CMAKE_SOURCE_DIR}/src/*.cpp"
         "${CMAKE_SOURCE_DIR}/src/*.ui"
    )
    # zh_CN.ts is used as template for new languages on Weblate. Therefore, all obsolute
    # strings should be removed.
    qt_add_lupdate(
      TS_FILES
      "${CMAKE_SOURCE_DIR}/resources/translations/zh_CN.ts"
      PLURALS_TS_FILE
      "${CMAKE_SOURCE_DIR}/resources/translations/en.ts"
      SOURCES
      ${SOURCES_TO_TRANSLATE}
      OPTIONS
      -no-obsolete
    )
    qt_add_lupdate(TS_FILES ${OTHER_TS_FILES} SOURCES ${SOURCES_TO_TRANSLATE})
    qt_add_lrelease(TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
    qt_add_resources(
      sane-break-app
      "translations"
      PREFIX
      "/i18n"
      BASE
      "${CMAKE_CURRENT_BINARY_DIR}"
      FILES
      "${QM_FILES}"
    )
  else()
    message(STATUS "Enable legacy translations")
    qt_add_translations(
      sane-break-app TS_FILES ${TS_FILES} LUPDATE_OPTIONS -no-obsolete
    )
  endif()
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DEFAULT_BUNDLE ON)
  else()
    set(DEFAULT_BUNDLE OFF)
  endif()
  set(BUNDLE_QT_TRANSLATIONS
      ${DEFAULT_BUNDLE}
      CACHE BOOL "Bundle translation files from QtBase"
  )
  if(${BUNDLE_QT_TRANSLATIONS})
    # Bundle Qt base translation files
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    execute_process(
      COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_TRANSLATIONS
      OUTPUT_VARIABLE QT_TRANSLATIONS_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(EXISTS "${QT_TRANSLATIONS_DIR}")
      message("-- Found Qt translations in ${QT_TRANSLATIONS_DIR}")
      set(COPIED_QT_BASE_QM_DIR ${CMAKE_CURRENT_BINARY_DIR}/translations/qtbase)
      set(COPIED_QT_BASE_QM_FILES "")
      make_directory(${COPIED_QT_BASE_QM_DIR})
      foreach(_ts_file ${TS_FILES})
        get_filename_component(_lang ${_ts_file} NAME_WLE)
        set(_qm_file ${QT_TRANSLATIONS_DIR}/qtbase_${_lang}.qm)
        set(_copied_qm_file ${COPIED_QT_BASE_QM_DIR}/${_lang}.qm)
        if(EXISTS ${_qm_file})
          list(APPEND COPIED_QT_BASE_QM_FILES ${_copied_qm_file})
          add_custom_command(
            OUTPUT ${_copied_qm_file}
            COMMAND ${CMAKE_COMMAND} -E copy ${_qm_file} ${_copied_qm_file}
            DEPENDS ${_qm_file}
          )
        endif()
      endforeach()
      message(STATUS "Adding ${QT_BASE_QM_FILES}")
      qt_add_resources(
        sane-break-app
        "qtbase-translations"
        PREFIX
        "i18n-qtbase"
        BASE
        ${COPIED_QT_BASE_QM_DIR}
        FILES
        ${COPIED_QT_BASE_QM_FILES}
      )
    else()
      message(WARNING "Qt translations not found")
    endif()
  endif()
else()
  message(STATUS "Disable translations because LinguistTools is not found")
endif()

