project(lib_simplex_io_tests)
enable_testing()
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
file(GLOB_RECURSE src "*.cpp")
add_executable(${PROJECT_NAME}
   ${src}
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME}
   ${CMAKE_PROJECT_NAME}
   GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

