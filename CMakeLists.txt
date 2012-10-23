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
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG_OUTPUT" )
elseif (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DEBUG_ENABLE_LOG" )
endif()
set ( MSG ${CMAKE_BUILD_TYPE} )
if (DESKTOP_APP)
  set ( MSG "${MSG} for desktop" )
else ()
  set ( MSG "${MSG} for tablet" )
endif()

#generate an automatic build/release number if available
#release numbers are generated from latest tag of the form [a-zA-Z_.]M.m
if( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=4 HEAD
      OUTPUT_VARIABLE TR_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    #replace dashes with points
    STRING(REGEX REPLACE "-" "." TR_VERSION ${TR_VERSION})
    #remove leading alphanumeric characters
    string(REGEX REPLACE "^[a-zA-Z_.]+(.*)" "\\1" TR_VERSION ${TR_VERSION})
  endif()
endif()
if(NOT TR_VERSION)
  message (WARNING "Cannot generate build/release number. Please set manually.")
  #set(TR_VERSION "2.1")
endif()

CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_BINARY_DIR}/src/config.h )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( src )
IF ( "Release" STREQUAL "${CMAKE_BUILD_TYPE}" )
  add_subdirectory ( help )
ENDIF()
IF ( "Debug" STREQUAL "${CMAKE_BUILD_TYPE}" )
  #	add_subdirectory ( test )
  message(STATUS "Add unit tests")
ENDIF()

message (STATUS "Build type: ${MSG}")
if (TR_VERSION)
  message (STATUS "Version: ${TR_VERSION}")
endif()
