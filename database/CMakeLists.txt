cmake_minimum_required(VERSION 3.21)
project(database)
add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(driver)
add_library(database STATIC  ${local_source} )





