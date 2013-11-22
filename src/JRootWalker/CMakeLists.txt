cmake_minimum_required (VERSION 2.6)

# Java

file (
    GLOB_RECURSE src_walker_java ${CMAKE_CURRENT_SOURCE_DIR}/raw "*.java"
)

find_package (Java REQUIRED)
include (UseJava)

add_jar (JRootWalker ${src_walker_java})

# Native
file (
    GLOB_RECURSE src_walker_c ${CMAKE_CURRENT_SOURCE_DIR}/raw "*.cpp"
)

include_directories (/usr/lib/jvm/java-7-openjdk-amd64/include/)
include_directories (${CMAKE_SOURCE_DIR}/src)

add_library (JRootWalkerNative SHARED ${src_walker_c})
target_link_libraries (JRootWalkerNative RootWalker)
