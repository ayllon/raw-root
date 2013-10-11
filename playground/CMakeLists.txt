cmake_minimum_required (VERSION 2.6)

add_definitions (-std=c++11)

add_subdirectory (root)
add_subdirectory (visitor)

# Test binary
set (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")

add_executable (test main.cpp)
target_link_libraries (test RootVisitor TMyClass)
