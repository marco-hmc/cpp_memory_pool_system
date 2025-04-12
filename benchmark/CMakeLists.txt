cmake_minimum_required(VERSION 3.1.0)

project(BenchMark VERSION 0.1.0)

file(GLOB LIB_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${LIB_SOURCES})
ADD_MODULE(${PROJECT_NAME} MemoryPool)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/utils
)