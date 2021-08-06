cmake_minimum_required(VERSION 3.21)
project(encode)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_definitions(-DUNICODE -D_UNICODE)
add_library(encode STATIC ${local_source})