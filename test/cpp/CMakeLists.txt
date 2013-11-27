cmake_minimum_required (VERSION 2.6)

# Test binary
include_directories (${CMAKE_SOURCE_DIR}/src)

add_definitions (-std=c++11 -DBUILD_PATH="${CMAKE_BINARY_DIR}/src/RootWalker/handlers/")

add_executable (test main.cpp)
target_link_libraries (test RootWalker)
