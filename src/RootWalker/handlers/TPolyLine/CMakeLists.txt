cmake_minimum_required (VERSION 2.6)

add_library (TPolyLine MODULE TPolyLine.cpp)
target_link_libraries (TPolyLine RootWalker ${ROOT_LIBRARIES})
