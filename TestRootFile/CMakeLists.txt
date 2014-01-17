cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package (Root REQUIRED)

include_directories (${ROOT_INCLUDE_DIR})
add_definitions (${ROOT_CFLAGS})

# Generate the dictionaries
add_custom_command (
    OUTPUT TMyClass-dictionary.cpp TMyClass-dictionary.h
    COMMAND rootcint TMyClass-dictionary.cpp -c ${CMAKE_CURRENT_SOURCE_DIR}/TMyClass.h
)

add_custom_command (
    OUTPUT TData-dictionary.cpp TData-dictionary.h
    COMMAND rootcint TData-dictionary.cpp -c ${CMAKE_CURRENT_SOURCE_DIR}/TData.h
)

# Build a shared object
set (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")

add_library (
    TMyClass MODULE
    TMyClass.cpp TMyClass-dictionary.cpp
    TData.cpp TData-dictionary.cpp
)
target_link_libraries (TMyClass ${ROOT_LIBRARIES})

set_target_properties (TMyClass PROPERTIES PREFIX "")


# Generate a file with some stupid content
add_custom_target (
    TestFile ALL
    DEPENDS TestFile.root
)

add_custom_command (
    OUTPUT TestFile.root
    COMMAND root -q -b "'${CMAKE_CURRENT_SOURCE_DIR}/GenerateFile.C(\"${CMAKE_CURRENT_BINARY_DIR}/TMyClass.so\",\"${CMAKE_CURRENT_BINARY_DIR}/TestFile.root\")'"
)
