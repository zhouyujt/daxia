cmake_minimum_required(VERSION 3.21)
project(net)

if (NET_HTTPS STREQUAL 1)
	include(FindOpenSSL.cmake)
	add_definitions(-DDAXIA_NET_SUPPORT_HTTPS)
	message(STATUS "net support https...")
endif()

add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(common)
add_library(net STATIC  ${local_source} ${common_source})
source_group(common FILES ${common_source})





