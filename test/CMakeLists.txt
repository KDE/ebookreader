project(TestTabletReader)

include_directories(${QT_QTTEST_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${OKULAR_INCLUDE_DIR}/okular
	${QT_MOBILITY_INCLUDE_DIR}/QtMobility
	${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_SOURCE_DIR}/test
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/3rdparty_linux/libchmfile)

set(TestTabletReader_SRCS main.cpp 
	${CMAKE_SOURCE_DIR}/src/okulardocument.cpp
	${CMAKE_SOURCE_DIR}/src/chmreply.cpp
	${CMAKE_SOURCE_DIR}/src/chmdocument.cpp)

QT4_WRAP_CPP(MOCS ${CMAKE_SOURCE_DIR}/src/chmreply.h main.h)

add_executable(testTabletReader ${TestTabletReader_SRCS} ${MOCS})
target_link_libraries(testTabletReader 
	${QT_QTTEST_LIBRARY} 
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${KDE4_KDECORE_LIBRARY}
	${CHM_LIBRARY}
	${QT_QTWEBKIT_LIBRARY}
	${PROJECT_BINARY_DIR}/../lib/libchmfile.a
	${OKULAR_LIBRARIES})
