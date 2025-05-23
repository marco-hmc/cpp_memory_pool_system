project(app)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})

ADD_DLL_MODULE(${PROJECT_NAME} MemoryPool)


find_package(benchmark REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

