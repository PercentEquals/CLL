cmake_minimum_required(VERSION 3.10)

add_library(CLL src/defined.cpp src/functions.cpp src/interpreter.cpp src/lexer.cpp src/var.cpp)
set_property(TARGET CLL PROPERTY CXX_STANDARD 14)
include_directories(${CMAKE_SOURCE_DIR}/CLL/include)

install(TARGETS CLL LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)