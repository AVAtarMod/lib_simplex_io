cmake_minimum_required(VERSION 3.10)
project(simplex_io)

# Always include the source and build directories in the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source_list "src/*.cpp")

# create the static library
add_library(${PROJECT_NAME} STATIC ${source_list})
target_include_directories(${PROJECT_NAME} PRIVATE "include/simplex_io")

get_directory_property(parent_dir_name PARENT_DIRECTORY)
if (NOT parent_dir_name)
   message(STATUS "Build as root project")

   set(target_build_type "debug")
   string(TOLOWER ${CMAKE_BUILD_TYPE} actual_build_type)
   if (actual_build_type STREQUAL target_build_type)
      message(STATUS "Tests (using Gtest) will be built")
      include(FetchContent)
      cmake_policy(SET CMP0135 OLD)
      FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
      )
      FetchContent_MakeAvailable(googletest)
      include(CTest)
      add_subdirectory(tests)
   endif()
else()
   message(STATUS "Build as sub-project")
endif()
