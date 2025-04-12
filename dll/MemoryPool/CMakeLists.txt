cmake_minimum_required(VERSION 3.1.0)

project(MemoryPool VERSION 0.1.0)

file(GLOB LIB_SOURCES "src/*.cpp")
include_directories("include")
add_library(${PROJECT_NAME} ${LIB_SOURCES})

set(PARENT_PROJECT_NAME ${PROJECT_NAME})
add_subdirectory(test)
