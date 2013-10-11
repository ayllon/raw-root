cmake_minimum_required (VERSION 2.6)

file (GLOB src_visitor "*.cpp")

set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")

add_library (RootVisitor SHARED ${src_visitor})
target_link_libraries (RootVisitor ${ROOT_LIBRARIES})

add_subdirectory (handlers)
