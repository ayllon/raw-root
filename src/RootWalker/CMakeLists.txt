cmake_minimum_required (VERSION 2.6)

add_definitions (-std=c++11)
set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")

file (GLOB src_visitor "*.cpp")

add_library (RootWalker SHARED ${src_visitor})
target_link_libraries (RootWalker ${ROOT_LIBRARIES})

add_subdirectory (handlers)
