project(tabletReader)

cmake_minimum_required ( VERSION 2.8.10 )

IF ( NOT CMAKE_BUILD_TYPE )
	set ( CMAKE_BUILD_TYPE Release )
ENDIF()

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH} )
find_package ( Qt4 4.7.0 REQUIRED )
find_package ( KDE4 4.7.0 REQUIRED )
find_package ( Okular REQUIRED )
find_package ( QtMobility 1.2.0 )

#work around for mer build
if (NOT QT_QTWEBKIT_INCLUDE_DIR)
  set (QT_QTWEBKIT_INCLUDE_DIR "/usr/include")
endif()

if (NOT QT_MOBILITY_FOUND)
  message(WARNING "Cannot find QtMobility.")
  set(NO_QTMOBILITY true)
endif()
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
elseif (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
  add_definitions(-DQT_DEBUG_ENABLE_LOG)
  #	add_subdirectory ( test )
  message(STATUS "Add unit tests")
endif()
add_subdirectory ( help )

option(DESKTOP_APP "Enable desktop application" on)
set ( MSG ${CMAKE_BUILD_TYPE} )
if (DESKTOP_APP)
  set ( MSG "${MSG} for desktop" )
else ()
  set ( MSG "${MSG} for tablet" )
endif()

#generate an automatic build/release number if available
#release numbers are generated from latest tag of the form [a-zA-Z_.]M.m
option(TIMESTAMP "Append timestamp to the version number" off)
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
    if(TIMESTAMP)
      #get commit date
      execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=\"%ci\" HEAD
        OUTPUT_VARIABLE TR_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      STRING(REGEX REPLACE "\"" "" TR_DATE ${TR_DATE})
      set(TR_VERSION "${TR_VERSION} @ ${TR_DATE}")
    endif()
  endif()
endif()
if(NOT TR_VERSION)
  set(TR_VERSION "3.0")
  message (WARNING "Cannot generate automatic build/release number. Using hard coded value.")
endif()

CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_BINARY_DIR}/src/config.h )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

add_subdirectory ( src )

message (STATUS "Build type: ${MSG}")
message (STATUS "Version: ${TR_VERSION}")
