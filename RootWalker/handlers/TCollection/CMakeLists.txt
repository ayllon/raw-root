cmake_minimum_required (VERSION 2.8)

add_library (TCollection MODULE TCollection.cpp)
target_link_libraries (TCollection RootWalker ${ROOT_LIBRARIES})
