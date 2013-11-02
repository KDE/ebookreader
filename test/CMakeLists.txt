project(TestTabletReader)

include_directories(${QT_QTTEST_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${OKULAR_INCLUDE_DIR}/okular
	${QT_MOBILITY_INCLUDE_DIR}/QtMobility
	${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_SOURCE_DIR}/test
	${CMAKE_SOURCE_DIR}/src)

set(TestTabletReader_SRCS main.cpp 
	${CMAKE_SOURCE_DIR}/src/okulardocument.cpp)

QT4_WRAP_CPP(MOCS main.h)

add_executable(testTabletReader ${TestTabletReader_SRCS} ${MOCS})
target_link_libraries(testTabletReader 
	${QT_QTTEST_LIBRARY} 
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${KDE4_KDECORE_LIBRARY}
	${QT_QTWEBKIT_LIBRARY}
	${OKULAR_LIBRARIES})
