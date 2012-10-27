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

set ( tabletReader_TRANS ${tabletReader_HDRS} ${tabletReader_SRCS} )
set ( tabletReader_TS
  ${CMAKE_SOURCE_DIR}/src/tabletReader_fr.ts
  ${CMAKE_SOURCE_DIR}/src/tabletReader_ro.ts
	)
QT4_CREATE_TRANSLATION(QMS ${tabletReader_TRANS} ${tabletReader_TS})

add_executable ( tabletReader ${tabletReader_SRCS} ${RSCS} ${MOCS} ${QMS} )
target_link_libraries ( tabletReader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${LIBS} )
install ( TARGETS tabletReader RUNTIME DESTINATION bin )
