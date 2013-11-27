cmake_minimum_required (VERSION 2.6)

# Java

file (
    GLOB_RECURSE src_walker_java ${CMAKE_CURRENT_SOURCE_DIR}/raw "*.java"
)

find_package (JNI REQUIRED)
message ("JNI header under ${JNI_INCLUDE_DIRS}")

find_package (Java REQUIRED)
include (UseJava)

add_jar (RootInferrer ${src_walker_java})

# Native
file (
    GLOB_RECURSE src_walker_c ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp"
)

include_directories (${JNI_INCLUDE_DIRS})
include_directories (${CMAKE_SOURCE_DIR}/src)

add_definitions (-std=c++11)

add_library (RootInferrerNative SHARED ${src_walker_c})
target_link_libraries (RootInferrerNative RootWalker)
