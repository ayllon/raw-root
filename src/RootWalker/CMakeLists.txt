cmake_minimum_required (VERSION 2.6)

add_definitions (-std=c++11)

file (GLOB src_visitor "*.cpp")

set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")

add_library (RootWalker SHARED ${src_visitor})
target_link_libraries (RootWalker ${ROOT_LIBRARIES} log4cxx)

add_subdirectory (handlers)
