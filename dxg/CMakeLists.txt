cmake_minimum_required(VERSION 3.21)
project(dxg)
add_definitions(-DUNICODE -D_UNICODE)
add_library(dxg STATIC 
client.cpp controller.cpp router.cpp scheduler.cpp server.cpp session.cpp sessions_manager.cpp 
common/basic_session.cpp common/http_parser.cpp common/parser.cpp common/shared_buffer.cpp
)