cmake_minimum_required (VERSION 2.6)

project (scidb-root)


set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package (Root REQUIRED)
find_package (Scidb REQUIRED)
