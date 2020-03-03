cmake_minimum_required(VERSION 3.10)

add_executable(CLL-interpreter src/console.cpp src/main.cpp)
set_property(TARGET CLL-interpreter PROPERTY CXX_STANDARD 14)
include_directories(${CMAKE_SOURCE_DIR}/Interpreter/include)
include_directories(${CMAKE_SOURCE_DIR}/CLL/include)
target_link_libraries(CLL-interpreter CLL)

install(TARGETS CLL-interpreter RUNTIME DESTINATION bin)