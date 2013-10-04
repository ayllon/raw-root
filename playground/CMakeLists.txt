cmake_minimum_required (VERSION 2.6)

add_definitions (-std=c++11)

add_subdirectory (root)
add_subdirectory (visitor)

# Test binary
add_executable (test main.cpp)
target_link_libraries (test rootvisitor TMyClass)
