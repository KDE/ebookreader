cmake_minimum_required ( VERSION 2.6 )

#available build types: Release, DesktopRelease, Debug, DesktopDebug, RelWithDebInfo, MinSizeRel, None
IF ( NOT CMAKE_BUILD_TYPE )
	set ( CMAKE_BUILD_TYPE Release )
ENDIF()

message (STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH} )
find_package ( Qt4 4.7.0 REQUIRED )
find_package ( QtMobility 1.2.0 )
find_package ( KDE4 REQUIRED 4.7.0 )
find_package ( Okular REQUIRED )
find_package ( LibChm REQUIRED )

if (NOT QT_MOBILITY_FOUND)
  set (CMAKE_CXX_FLAGS "-DNO_MOBILITY ${CMAKE_CXX_FLAGS}")
  message(WARNING "Cannot find QtMobility.")
endif()
set ( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT" )
set ( CMAKE_CXX_FLAGS_DESKTOPRELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT -DDESKTOP_APP" )
set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_DEBUG_ENABLE_LOG -ggdb" )
set ( CMAKE_CXX_FLAGS_DESKTOPDEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_DEBUG_ENABLE_LOG -DDESKTOP_APP -ggdb" )


include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( src )
IF (("Debug" STREQUAL "${CMAKE_BUILD_TYPE}") OR ("DesktopDebug" STREQUAL "${CMAKE_BUILD_TYPE}"))
	add_subdirectory ( test )
ENDIF()

