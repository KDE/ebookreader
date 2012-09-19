project ( tabletReader )

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

set ( tabletReader_HDRS
	logger.h
	document.h
	chmdocument.h
	okulardocument.h
	)

set ( tabletReader_SRCS
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

set ( tabletReader_RSCS
	images.qrc
	)
QT4_ADD_RESOURCES(RSCS ${tabletReader_RSCS})

set ( tabletReader_TRS
	tabletReader_fr.ts
	tabletReader_ro.ts
	)
QT4_ADD_TRANSLATION(TRS ${tabletReader_TRS})

set ( tabletReader_MOCS
	documentwidget.h
	window.h
	SlidingStackedWidget.h
	flickable.h
	worker.h
	filebrowsermodel.h
	chmreply.h
	)
if (QT_MOBILITY_FOUND)
  QT4_WRAP_CPP(MOCS ${tabletReader_MOCS})
else()
  QT4_WRAP_CPP(MOCS ${tabletReader_MOCS} OPTIONS -DNO_MOBILITY)
endif()

add_executable ( tabletReader ${tabletReader_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( tabletReader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${LIBS} )
install ( TARGETS tabletReader RUNTIME DESTINATION bin )
