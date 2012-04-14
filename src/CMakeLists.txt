project ( tabletReader )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDECLARATIVE_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTWEBKIT_INCLUDE_DIR}
	${QT_MOBILITY_INCLUDE_DIR}/QtMobility
	${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR}
	${POPPLER_INCLUDE_DIR}
	${POPPLER_QT4_INCLUDE_DIR}/poppler/qt4
	${CHM_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/../3rdparty_linux/libchmfile
	${DJVU_INCLUDE_DIR}/libdjvu )
set ( LIBS ${LIBS} 
	${QT_MOBILITY_SYSTEMINFO_LIBRARY}
	${POPPLER_LIBRARY}
	${POPPLER_QT4_LIBRARY}
	${CHM_LIBRARY}
	${DJVU_LIBRARY} )

set ( tabletReader_HDRS
	logger.h
	pdfdocument.h
	document.h
	djvudocument.h
	chmdocument.h
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
	pdfdocument.cpp
	djvudocument.cpp
	kdjvu.cpp
	chmdocument.cpp
	chmreply.cpp
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
	kdjvu.h
	chmreply.h
	)
QT4_WRAP_CPP(MOCS ${tabletReader_MOCS})

add_executable ( tabletReader ${tabletReader_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( tabletReader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${LIBS} )
