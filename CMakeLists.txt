cmake_minimum_required ( VERSION 2.6 )

#available build types: Release, DesktopRelease, Debug, DesktopDebug, RelWithDebInfo, MinSizeRel, None
IF ( NOT CMAKE_BUILD_TYPE )
	set ( CMAKE_BUILD_TYPE Release )
ENDIF()

message ("-- Build type: " ${CMAKE_BUILD_TYPE})

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH} )
IF ("Release" STREQUAL "${CMAKE_BUILD_TYPE}")
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG_OUTPUT" )
ENDIF()
IF ("DesktopRelease" STREQUAL "${CMAKE_BUILD_TYPE}")
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG_OUTPUT -DDESKTOP_APP" )
ENDIF()
IF ("Debug" STREQUAL "${CMAKE_BUILD_TYPE}")
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DEBUG_ENABLE_LOG -ggdb" )
ENDIF()
IF ("DesktopDebug" STREQUAL "${CMAKE_BUILD_TYPE}")
	set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DEBUG_ENABLE_LOG -DDESKTOP_APP -ggdb" )
ENDIF()

find_package ( Qt4 4.7.0 REQUIRED )
find_package ( QtMobility 1.2.0 REQUIRED )
find_package ( KDE4 REQUIRED 4.7.0 )
find_package ( Okular REQUIRED )
find_package ( LibChm REQUIRED )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( 3rdparty_linux/libchmfile )
add_subdirectory ( src )
add_subdirectory ( src/help )
IF (("Debug" STREQUAL "${CMAKE_BUILD_TYPE}") OR ("DesktopDebug" STREQUAL "${CMAKE_BUILD_TYPE}"))
	add_subdirectory ( test )
ENDIF()

