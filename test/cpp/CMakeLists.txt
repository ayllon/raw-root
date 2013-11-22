cmake_minimum_required (VERSION 2.6)

# Test binary
include_directories (${CMAKE_SOURCE_DIR}/src)
set (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")

add_definitions (-std=c++11 -DBUILD_PATH="${CMAKE_BINARY_DIR}/src/RootWalker/handlers/")

add_executable (test main.cpp)
target_link_libraries (test RootWalker TMyClass log4cxx)
