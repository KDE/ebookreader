cmake_minimum_required ( VERSION 2.8.6 )

project(tabletReader)

IF ( NOT CMAKE_BUILD_TYPE )
	set ( CMAKE_BUILD_TYPE Release )
ENDIF()

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH} )
find_package ( Qt4 4.7.0 REQUIRED )
find_package ( KDE4 REQUIRED 4.7.0 )
find_package ( Okular REQUIRED )
find_package ( LibChm REQUIRED )
find_package ( QtMobility 1.2.0 )

if (NOT QT_MOBILITY_FOUND)
  set (NO_MOBILITY)
  message(WARNING "Cannot find QtMobility.")
endif()
set ( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT" )
set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_DEBUG_ENABLE_LOG" )
set ( MSG "Build type: ${CMAKE_BUILD_TYPE}" )
if (DESKTOP_APP)
  set ( MSG "${MSG} for desktop" )
endif()

CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_BINARY_DIR}/src/config.h )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( src )
add_subdirectory ( help )
IF ( "Debug" STREQUAL "${CMAKE_BUILD_TYPE}" )
  #	add_subdirectory ( test )
  message(STATUS "Add unit tests")
ENDIF()

message (STATUS ${MSG})
