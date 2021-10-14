cmake_minimum_required(VERSION 3.21)
project(coroutine)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_definitions(-DUNICODE -D_UNICODE)
add_library(coroutine STATIC ${local_source})
