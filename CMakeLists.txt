cmake_minimum_required(VERSION 3.15)
project(scanner_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



option(BUILD_TESTS "Build tests" ON)


add_subdirectory(src/core)
add_subdirectory(src/app)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()