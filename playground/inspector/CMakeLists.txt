cmake_minimum_required (VERSION 2.6)

file (GLOB src_inspector "*.cpp")

# Need these flags to get linked against all Root libraries, and not
# only those used
set (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")

add_executable(inspector ${src_inspector})
target_link_libraries (inspector ${ROOT_LIBRARIES})
