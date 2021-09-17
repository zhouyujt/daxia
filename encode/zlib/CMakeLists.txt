cmake_minimum_required(VERSION 3.21)
project(encode)

file(GLOB local_source
"zlib/*.h"
"zlib/*.c"
"zlib/contrib/masmx64/inffas8664.c"
"zlib/contrib/minizip/ioapi.c"
"zlib/contrib/minizip/unzip.c"
"zlib/contrib/minizip/zip.c"
)

add_definitions(-DUNICODE -D_UNICODE)
include_directories(zlib)
add_library(zlib STATIC ${local_source})