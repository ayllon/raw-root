cmake_minimum_required (VERSION 2.6)

file (GLOB src_visitor "*.cpp")

set (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")

add_executable(visitor ${src_visitor})
target_link_libraries (visitor ${ROOT_LIBRARIES} TMyClass)
