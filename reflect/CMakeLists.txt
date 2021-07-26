cmake_minimum_required(VERSION 3.21)
project(reflect)
add_definitions(-DUNICODE -D_UNICODE)
add_library(reflect STATIC 
reflect.hpp reflect_base.cpp
)