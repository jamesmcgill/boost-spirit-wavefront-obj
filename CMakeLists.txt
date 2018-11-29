cmake_minimum_required(VERSION 3.9.2)
project (ObjParser VERSION 0.0.1 DESCRIPTION "wavefront obj loader")

if (MSVC)
  add_compile_options(/W4 /permissive-)
endif()

# Dependencies

# On MSVC CMake will build everything (by default) with the shared crt (/MD)
# There is no defined default for what crt boost will use.
# This flag ensures boost also uses the shared crt on all platforms
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.55.0 REQUIRED)

add_subdirectory(third_party/fmt-5.2.1 EXCLUDE_FROM_ALL)


# Library
add_library(ObjParser source/ObjParser.cpp)
target_include_directories(ObjParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ObjParser PUBLIC Boost::boost)
target_compile_features(ObjParser PUBLIC cxx_std_17)

# Executable
add_executable(ObjParserMain source/main.cpp)
target_link_libraries(ObjParserMain
    PRIVATE ObjParser fmt-header-only)

set_target_properties(ObjParserMain PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_command(TARGET ObjParserMain POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:ObjParserMain>/assets")

# GTest + GMock
# On MSVC, gtest will try to use the static crt (/MT) which is different
# from CMake's default (/MD).
# This flag ensures gtest uses the shared crt to match with everything else
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest-1.8.1")

# Tests
enable_testing()
add_executable(tests tests/main.cpp)
target_link_libraries(tests gmock_main ObjParser)
add_test(AllTests tests)
