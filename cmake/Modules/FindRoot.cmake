#
# This module detects if root is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
# 
# ROOT_INCLUDE_DIR
# ROOT_CFLAGS
# ROOT_VERSION
# ROOT_FOUND
#

# -----------------------------------------------------
# root-config binary
# -----------------------------------------------------
find_program (ROOT_CONFIG
    NAMES root-config
)

if (ROOT_CONFIG)
    # -----------------------------------------------------
    # ROOT CFLAGS
    # -----------------------------------------------------
    execute_process (
        COMMAND ${ROOT_CONFIG} --cflags
        OUTPUT_VARIABLE ROOT_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
        
    # -----------------------------------------------------
    # ROOT Libraries
    # -----------------------------------------------------
    execute_process (
        COMMAND ${ROOT_CONFIG} --libs
        OUTPUT_VARIABLE ROOT_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # -----------------------------------------------------
    # ROOT Include directories
    # -----------------------------------------------------
    execute_process (
        COMMAND ${ROOT_CONFIG} --incdir
        OUTPUT_VARIABLE ROOT_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (ROOT_CONFIG)

# -----------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set ROOT_FOUND to TRUE if 
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(root DEFAULT_MSG ROOT_INCLUDE_DIR ROOT_LIBRARIES ROOT_CFLAGS)
mark_as_advanced(ROOT_INCLUDE_DIR ROOT_LIBRARIES ROOT_CFLAGS)
