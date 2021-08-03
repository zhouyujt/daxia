cmake_minimum_required(VERSION 3.21)
project(database)

include(FindMySQL.cmake)

add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(driver)
add_library(database STATIC  ${local_source} ${driver_source} )
source_group(driver FILES ${driver_source})





