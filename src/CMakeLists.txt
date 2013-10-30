cmake_minimum_required (VERSION 2.6)

add_subdirectory (RootWalker)

include_directories ("${CURRENT_SOURCE_DIR}/RootWalker/")
add_subdirectory (ScidbPlugin)
