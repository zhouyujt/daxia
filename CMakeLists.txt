cmake_minimum_required(VERSION 3.21)
project(daxia)

IF(CMAKE_CL_64)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RLEASE 	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)

	IF (CMAKE_BUILD_TYPE STREQUAL Debug)
		set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)   
	ELSE()
		set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)   
	ENDIF()
ELSE(CMAKE_CL_64)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RLEASE 	${CMAKE_CURRENT_SOURCE_DIR}/libs/release)

	IF (CMAKE_BUILD_TYPE STREQUAL Debug)
		set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)   
	ELSE()
		set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/release)   
	ENDIF()
ENDIF(CMAKE_CL_64)

#cmake -DCMAKE_BUILD_TYPE=Debug/Release
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory("dxg" dxg)
add_subdirectory("encode" encode)
add_subdirectory("reflect" reflect)
add_subdirectory("win32" win32)

