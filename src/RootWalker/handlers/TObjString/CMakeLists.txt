cmake_minimum_required (VERSION 2.6)

add_library (TObjString MODULE TObjString.cpp)
target_link_libraries (TObjString RootWalker ${ROOT_LIBRARIES})
