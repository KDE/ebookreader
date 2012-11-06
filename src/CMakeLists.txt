project ( tabletReader )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDECLARATIVE_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTWEBKIT_INCLUDE_DIR} 
	${QT_MOBILITY_INCLUDE_DIR}/QtMobility ${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR}
	${OKULAR_INCLUDE_DIR}/okular )
set ( LIBS ${LIBS}
	${QT_QTNETWORK_LIBRARY}
	${QT_MOBILITY_SYSTEMINFO_LIBRARY}
	${OKULAR_CORE_LIBRARY} 
	${KDE4_KDECORE_LIBRARY} )

file ( GLOB tabletReader_HDRS *.h )

file ( GLOB tabletReader_SRCS *.cpp)

set ( tabletReader_RSCS images.qrc )
QT4_ADD_RESOURCES(RSCS ${tabletReader_RSCS})

set ( tabletReader_MOCS
  okulardocument.h
	documentwidget.h
	window.h
	SlidingStackedWidget.h
	flickable.h
	filebrowsermodel.h
	)
QT4_WRAP_CPP(MOCS ${tabletReader_MOCS})

if (NOT WIN32)
  #don't generate translations on Windows (use the existing ones)
  set ( tabletReader_TRANS ${tabletReader_HDRS} ${tabletReader_SRCS} )
  set ( tabletReader_TS
    ${CMAKE_SOURCE_DIR}/src/tabletReader_fr.ts
    ${CMAKE_SOURCE_DIR}/src/tabletReader_ro.ts
	  )
  QT4_CREATE_TRANSLATION(QMS ${tabletReader_TRANS} ${tabletReader_TS})
endif()

add_executable ( tabletReader ${tabletReader_SRCS} ${RSCS} ${MOCS} ${QMS} )
target_link_libraries ( tabletReader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${LIBS} )
if (WIN32)
  set_target_properties(tabletReader PROPERTIES WIN32_EXECUTABLE TRUE)
  target_link_libraries ( tabletReader ${QT_QTMAIN_LIBRARY} )
endif()

install ( TARGETS tabletReader RUNTIME DESTINATION bin )

if (EXISTS ${CMAKE_BINARY_DIR}/src/tabletReader_ro.qm)
  configure_file(${CMAKE_BINARY_DIR}/src/tabletReader_ro.qm ${CMAKE_SOURCE_DIR}/src/tabletReader_ro.qm COPYONLY)
endif()
if (EXISTS ${CMAKE_BINARY_DIR}/src/tabletReader_fr.qm)
  configure_file(${CMAKE_BINARY_DIR}/src/tabletReader_fr.qm ${CMAKE_SOURCE_DIR}/src/tabletReader_fr.qm COPYONLY)
endif()

#setup installers
set(CPACK_PACKAGE_NAME "tabletReader")
set(CPACK_PACKAGE_VENDOR "Bogdan Cristea")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "e-book reader for touch enabled devices")
set(CPACK_PACKAGE_VERSION ${TR_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/Copyright.txt)
if (WIN32)
  if (NOT DEPS_FILE)
    set(DEPS_FILE "${CMAKE_SOURCE_DIR}/win/dep_list.txt")
  endif()
  if (NOT KDE_ROOT)
    set(KDE_ROOT "C:\\\\kderoot")
  endif()
  if (NOT EXISTS ${DEPS_FILE})
    message(FATAL_ERROR "Please provide the dependences file in DEPS_FILE")
  endif()
  if (NOT IS_DIRECTORY ${KDE_ROOT})
    message(FATAL_ERROR "Please provide the KDE root path in KDE_ROOT")
  endif()
  FILE(STRINGS ${DEPS_FILE} contents)
  foreach(line ${contents})
    if (${line} MATCHES "^[a-z]")
      STRING(REGEX REPLACE "\\\\" "/" dep_path ${line})
      STRING(REGEX MATCH "(.*)[/\\]" DIR ${dep_path})
      install(FILES ${KDE_ROOT}/${dep_path} DESTINATION ${DIR})
    endif()
  endforeach()
  install(FILES ${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico DESTINATION bin)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "tabletReader.exe")
  set(CPACK_NSIS_CONTACT "cristeab@gmail.com")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\tabletReader.exe")
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://projects.kde.org/projects/playground/edu/ebookreader")
  set(CPACK_NSIS_CONTACT "Bogdan Cristea <cristeab\@gmail.com>")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\kbuildsycoca4.exe\\\"'
  CreateShortCut \\\"$DESKTOP\\\\tabletReader.lnk\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.ico\\\"
  CreateShortCut \\\"$SMPROGRAMS\\\\tabletReader.lnk\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.ico\\\"")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\tabletReader.lnk\\\"
  Delete \\\"$SMPROGRAMS\\\\tabletReader.lnk\\\"")
  if (EXISTS ${CMAKE_BINARY_DIR}/tabletReader-2.3-win32.exe)
    configure_file(${CMAKE_BINARY_DIR}/tabletReader-2.3-win32.exe ${CMAKE_SOURCE_DIR}/win/tabletReader-2.3-win32.exe COPYONLY)
  endif()
endif()
include(CPack)
