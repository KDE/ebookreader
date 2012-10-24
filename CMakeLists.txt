project(tabletReader)

cmake_minimum_required ( VERSION 2.8.6 )

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
  message(WARNING "Cannot find QtMobility.")
  add_definitions(-DNO_QTMOBILITY)
endif()
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
  add_subdirectory ( help )
elseif (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
  add_definitions(-DQT_DEBUG_ENABLE_LOG)
  #	add_subdirectory ( test )
  message(STATUS "Add unit tests")
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
  set(TR_VERSION "2.3")
  message (WARNING "Cannot generate automatic build/release number. Set manually.")
endif()

CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_BINARY_DIR}/src/config.h )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( src )

message (STATUS "Build type: ${MSG}")
if (TR_VERSION)
  message (STATUS "Version: ${TR_VERSION}")
endif()
