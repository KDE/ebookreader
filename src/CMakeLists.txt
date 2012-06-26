project ( atReader )

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDECLARATIVE_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTWEBKIT_INCLUDE_DIR} 
	${QT_MOBILITY_INCLUDE_DIR}/QtMobility ${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR}
	${OKULAR_INCLUDE_DIR}/okular
	${CHM_INCLUDE_DIR} ../3rdparty_linux/libchmfile )
set ( LIBS ${LIBS}
	${QT_QTNETWORK_LIBRARY}
	${QT_MOBILITY_SYSTEMINFO_LIBRARY}
	${CHM_LIBRARY} 
	${PROJECT_BINARY_DIR}/../lib/libchmfile.a
	${OKULAR_CORE_LIBRARY} 
	${KDE4_KDECORE_LIBRARY} )

set ( atReader_HDRS
	logger.h
	document.h
	chmdocument.h
	okulardocument.h
	)

set ( atReader_SRCS
	documentwidget.cpp
	main.cpp
	window.cpp
	SlidingStackedWidget.cpp
	flickable.cpp
	worker.cpp
	logger.cpp
	filebrowsermodel.cpp
	chmdocument.cpp
	chmreply.cpp
	okulardocument.cpp
	)

set ( atReader_RSCS
	images.qrc
	)
QT4_ADD_RESOURCES(RSCS ${atReader_RSCS})

set ( atReader_TRS
	atReader_fr.ts
	atReader_ro.ts
	)
QT4_ADD_TRANSLATION(TRS ${atReader_TRS})

set ( atReader_MOCS
	documentwidget.h
	window.h
	SlidingStackedWidget.h
	flickable.h
	worker.h
	filebrowsermodel.h
	chmreply.h
	)
QT4_WRAP_CPP(MOCS ${atReader_MOCS})

add_executable ( atReader ${atReader_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( atReader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${LIBS} )
install ( TARGETS atReader RUNTIME DESTINATION bin )
install (FILES ${CMAKE_SOURCE_DIR}/atReader.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/128x128/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128)
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/64x64/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/64x64)
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/48x48/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48)
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/32x32/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/32x32)
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/22x22/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/22x22)
install (FILES ${CMAKE_SOURCE_DIR}/src/icons/16x16/atReader.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/16x16)
