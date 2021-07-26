cmake_minimum_required(VERSION 3.21)
project(win32)
add_definitions(-DUNICODE -D_UNICODE)
add_library(win32 STATIC 
access_token.cpp path.cpp process.cpp processes_manager.cpp
)