cmake_minimum_required(VERSION 3.21)
project(net)
add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(common)
add_library(net STATIC  ${local_source} ${common_source})
source_group(common FILES ${common_source})





