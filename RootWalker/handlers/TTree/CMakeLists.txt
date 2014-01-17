cmake_minimum_required (VERSION 2.8)

add_library (TTree MODULE TTree.cpp)
target_link_libraries (TTree RootWalker ${ROOT_LIBRARIES})
