cmake_minimum_required (VERSION 2.6)

set (scidb_root_src
    plugin.cpp
    version/LogicalRootVersion.cpp
    version/PhysicalRootVersion.cpp
    loader/LogicalRootLoader.cpp
    loader/PhysicalRootLoader.cpp
)

add_library(root SHARED ${scidb_root_src})

target_link_libraries (root RootWalker ${ROOT_LIBRARIES} ${SCIDB_LIBRARIES})
