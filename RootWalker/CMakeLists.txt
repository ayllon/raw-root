cmake_minimum_required (VERSION 2.8)

project (RootWalker)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")
find_package (Boost REQUIRED)
find_package (Root REQUIRED)

include_directories (
    ${Boost_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIR}
)
add_definitions (${ROOT_CFLAGS})

add_subdirectory (src)
add_subdirectory (test)
