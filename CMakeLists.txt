cmake_minimum_required(VERSION 3.10)
project(lib_simplex_io)

# Always include the source and build directories in the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source_list "src/*.cpp")

# create the static library
add_library(${PROJECT_NAME} STATIC ${source_list})
target_include_directories(${PROJECT_NAME} PRIVATE "include/lib_simplex_io")
