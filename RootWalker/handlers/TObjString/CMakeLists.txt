cmake_minimum_required (VERSION 2.8)

add_library (TObjString MODULE TObjString.cpp)
target_link_libraries (TObjString RootWalker ${ROOT_LIBRARIES})
