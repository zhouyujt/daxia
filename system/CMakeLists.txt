cmake_minimum_required(VERSION 3.21)
project(system)
add_definitions(-DUNICODE -D_UNICODE)

file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

add_subdirectory(threadpool)
add_subdirectory(windows)
add_library(system STATIC  ${local_source} ${threadpool_source} ${windows_source})
source_group(threadpool FILES ${threadpool_source})
source_group(windows FILES ${windows_source})






