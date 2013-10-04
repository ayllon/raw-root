cmake_minimum_required (VERSION 2.6)

project (scidb-root)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package (Boost REQUIRED)
find_package (Root REQUIRED)
find_package (Scidb REQUIRED)

include_directories (
    ${Boost_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIR}
    ${SCIDB_INCLUDES}
)
add_definitions (${ROOT_CFLAGS} ${SCIDB_CXX_FLAGS} -std=c++11)

add_subdirectory (src)
add_subdirectory (playground)
