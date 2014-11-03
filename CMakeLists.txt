project(CreamScript)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
cmake_minimum_required(VERSION 2.8)
file(GLOB HEADERS src/*.hpp src/*.h)
file(GLOB SOURCES src/*.cpp)
aux_source_directory(. SRC_LIST)
aux_source_directory(src SRC)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS} ${SOURCES})

