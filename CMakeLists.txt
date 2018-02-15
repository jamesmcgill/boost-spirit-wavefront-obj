cmake_minimum_required(VERSION 3.10)
project (ObjLoader VERSION 0.0.1 DESCRIPTION "wavefront obj loader")

find_package(Boost 1.66.0)
include_directories(${Boost_INCLUDE_DIRS})

if (MSVC)
  add_compile_options(/W4 /permissive-)
endif()

# Library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(ObjLoader source/lib.cpp)
target_compile_features(ObjLoader PUBLIC cxx_std_17)

# Executable
add_executable(ObjLoaderMain source/main.cpp source/fmt/format.cc)
target_link_libraries(ObjLoaderMain ObjLoader)
target_compile_features(ObjLoaderMain PUBLIC cxx_std_17)
set_target_properties(ObjLoaderMain PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_command(TARGET ObjLoaderMain POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:ObjLoaderMain>/assets")

# GTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest")

# Tests
add_executable(tests tests/main.cpp)
target_link_libraries(tests gtest gtest_main ObjLoader)
add_test(AllTests tests)
