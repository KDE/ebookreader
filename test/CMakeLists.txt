project(TestTabletReader)

cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.7.0 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(Okular REQUIRED)

include_directories(${QT_QTTEST_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}	
	${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/../src)

set(TestTabletReader_SRCS main.cpp ../src/okulardocument.cpp)

QT4_WRAP_CPP(MOCS main.h)

add_executable(testTabletReader ${TestTabletReader_SRCS} ${MOCS})
target_link_libraries(testTabletReader 
	${QT_QTTEST_LIBRARY} 
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${KDE4_KDECORE_LIBRARY}
	${OKULAR_LIBRARIES})
