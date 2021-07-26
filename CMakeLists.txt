cmake_minimum_required(VERSION 3.21)
project(daxia)

IF(CMAKE_CL_64)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_GENERATOR}/x64/debug)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RLEASE 	${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_GENERATOR}x64/release)
ELSE(CMAKE_CL_64)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_GENERATOR}/debug)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RLEASE 	${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_GENERATOR}/release)
ENDIF(CMAKE_CL_64)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory("dxg" dxg)
add_subdirectory("encode" encode)
add_subdirectory("reflect" reflect)
add_subdirectory("win32" win32)

