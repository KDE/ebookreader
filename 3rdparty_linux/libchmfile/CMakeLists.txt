project( chmfile )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	../../src)
set ( chmfile_HDRS
	bitfiddle.h
	libchmfile.h
	libchmfileimpl.h
	libchmtextencoding.h
	libchmtocimage.h
	libchmurlfactory.h
	libchmsearchengine_impl.h
	)

set ( chmfile_SRCS
	libchmfile.cpp
	libchmfileimpl.cpp
	libchmfile_search.cpp
	libchmtextencoding.cpp
	libchmtocimage.cpp
	libchmsearchengine.cpp
	libchmsearchengine_impl.cpp
	libchmsearchengine_indexing.cpp
	)

set ( chmfile_MOCS
	libchmsearchengine.h
	libchmsearchengine_indexing.h
	)
QT4_WRAP_CPP(MOCS ${chmfile_MOCS})

add_library ( chmfile SHARED ${chmfile_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( chmfile  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
