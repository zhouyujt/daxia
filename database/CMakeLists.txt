cmake_minimum_required(VERSION 3.21)
project(database)

include(FindMySQL.cmake)

add_definitions(-DUNICODE -D_UNICODE)
if (ORM_MYSQL STREQUAL 1)
	add_definitions(-D__DAXIA_DATABASE_DRIVER_USE_MYSQL)
	message(STATUS "Database use orm MySQL...")
endif()

if (ORM_SQLITE STREQUAL 1)
	add_definitions(-D__DAXIA_DATABASE_DRIVER_USE_SQLITE)
	message(STATUS "Database use orm SQLITE...")
endif()

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(driver)
add_library(database STATIC  ${local_source} ${driver_source} )
source_group(driver FILES ${driver_source})





