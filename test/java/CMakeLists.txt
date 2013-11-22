cmake_minimum_required (VERSION 2.6)

find_package (Java REQUIRED)
include (UseJava)

set (CMAKE_JAVA_INCLUDE_PATH "${CMAKE_BINARY_DIR}/src/JRootWalker/JRootWalker.jar")

set (CMAKE_JAVA_JAR_ENTRY_POINT Main)
add_jar (JavaTest Main.java)
