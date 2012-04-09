cmake_minimum_required ( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH} )
add_definitions ( -Wall )
find_package ( Qt4 4.7.0 REQUIRED )
find_package ( QtMobility 1.2.0 REQUIRED )
find_package ( LibPoppler REQUIRED )
find_package ( LibPopplerQt4 REQUIRED )
find_package ( LibChm REQUIRED )
find_package ( LibDjvu REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
add_subdirectory ( 3rdparty_linux/libchmfile )
add_subdirectory ( src )

