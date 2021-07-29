cmake_minimum_required(VERSION 3.21)
project(database_driver)
add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_library(database_driver STATIC  ${local_source} )






