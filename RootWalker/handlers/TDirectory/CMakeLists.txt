cmake_minimum_required (VERSION 2.8)

add_library (TDirectory MODULE TDirectory.cpp)
target_link_libraries (TDirectory RootWalker ${ROOT_LIBRARIES})
