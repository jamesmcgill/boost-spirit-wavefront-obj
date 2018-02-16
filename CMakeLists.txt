cmake_minimum_required(VERSION 3.10)
project (ObjParser VERSION 0.0.1 DESCRIPTION "wavefront obj loader")

find_package(Boost 1.66.0)
include_directories(${Boost_INCLUDE_DIRS})

if (MSVC)
  add_compile_options(/W4 /permissive-)
endif()

# Library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(ObjParser source/ObjParser.cpp)
target_compile_features(ObjParser PUBLIC cxx_std_17)

# Executable
add_executable(ObjParserMain source/main.cpp source/fmt/format.cc)
target_link_libraries(ObjParserMain ObjParser)
target_compile_features(ObjParserMain PUBLIC cxx_std_17)
set_target_properties(ObjParserMain PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_command(TARGET ObjParserMain POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:ObjParserMain>/assets")

# GTest + GMock
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest")
target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
  "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")

# Tests
add_executable(tests tests/main.cpp)
target_link_libraries(tests gmock_main ObjParser)
add_test(AllTests tests)
