cmake_minimum_required (VERSION 2.6)

file (GLOB src_visitor "*.cpp")

set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")

add_library(rootvisitor SHARED ${src_visitor})
target_link_libraries(rootvisitor ${ROOT_LIBRARIES})
