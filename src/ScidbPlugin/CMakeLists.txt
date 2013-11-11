cmake_minimum_required (VERSION 2.6)

set (scidb_root_src
    plugin.cpp
    version/LogicalRootVersion.cpp
    version/PhysicalRootVersion.cpp
    loader/LogicalRootLoader.cpp
    loader/PhysicalRootLoader.cpp
)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../RootWalker/")

set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")

add_library(root SHARED ${scidb_root_src})
target_link_libraries (root RootWalker ${ROOT_LIBRARIES} ${SCIDB_LIBRARIES})
