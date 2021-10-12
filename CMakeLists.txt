set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_minimum_required(VERSION 3.21)
project(daxia)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(IS64 false)	
if(MSVC)
	if(CMAKE_CL_64)
		set(IS64 true)	
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P)
		set(IS64 true)
	endif()
endif()

if(IS64)
	if(MSVC AND MT STREQUAL 1)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debugMT)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/releaseMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debugMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/releaseMT)
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)
	endif()
else()
	if(MSVC AND MT STREQUAL 1)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debugMT)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/releaseMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debugMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/releaseMT)
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/release)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/release)
	endif()
endif()

#cmake -DCMAKE_BUILD_TYPE=Debug/Release
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -W3 -g -ggdb ")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -W3")

if(MSVC AND MT STREQUAL 1)
    add_compile_options(
        $<$<CONFIG:>:/MT> #----------|
        $<$<CONFIG:Debug>:/MTd>#--|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})
#find_package(MySQL)
#include_directories(${MySQL_INCLUDE_DIRS})

add_subdirectory(net)
add_subdirectory(encode)
add_subdirectory(reflect)
add_subdirectory(database)
add_subdirectory(system)

