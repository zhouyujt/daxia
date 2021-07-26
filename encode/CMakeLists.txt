cmake_minimum_required(VERSION 3.21)
project(encode)
add_definitions(-DUNICODE -D_UNICODE)
add_library(encode STATIC 
base64.cpp json.cpp md5.cpp strconv.cpp
)