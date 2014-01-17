cmake_minimum_required (VERSION 2.8)

add_library (Vector MODULE Vector.cpp)
target_link_libraries (Vector RootWalker ${ROOT_LIBRARIES})
