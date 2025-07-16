# Adds a `format` target to the project to run clang-format.
#
# This module provides a single function, `add_clang_format_target`, which
# creates a custom CMake target named `format`. When built, this target will
# find and format all C/C++ source and header files within a given set of
# directories.
#
# USAGE:
#
# ~~~
# add_clang_format_target(DIRECTORIES <dir1> [<dir2>...])
# ~~~
#
# PARAMETERS:
#
# * `DIRECTORIES` - A space-separated list of directories to search recursively
#   for files to format.
#
# EXAMPLE:
#
# ~~~
# # In your CMakeLists.txt
# include(ClangFormatProject)
#
# add_clang_format_target(
#     DIRECTORIES
#         ${CMAKE_SOURCE_DIR}/src
#         ${CMAKE_SOURCE_DIR}/include
# )
# ~~~

include(CMakeParseArguments)

function(add_clang_format_target)
  find_package(ClangFormat REQUIRED)
  if(NOT ClangFormat_FOUND)
    return()
  endif()

  cmake_parse_arguments(
    ARG # Prefix for parsed arguments
    "" # No optional arguments
    "" # No single-value arguments
    "DIRECTORIES" # Multi-value arguments
    ${ARGN}
  )

  if(NOT ARG_DIRECTORIES)
    message(
      FATAL_ERROR "add_clang_format_target() called without any DIRECTORIES."
    )
    return()
  endif()

  set(ALL_FORMAT_FILES)

  foreach(dir IN LISTS ARG_DIRECTORIES)
    file(
      GLOB_RECURSE
      dir_files
      "${dir}/*.h"
      "${dir}/*.hpp"
      "${dir}/*.c"
      "${dir}/*.cpp"
      "${dir}/*.cc"
    )
    list(APPEND ALL_FORMAT_FILES ${dir_files})
  endforeach()

  list(REMOVE_DUPLICATES ALL_FORMAT_FILES)

  if(NOT ALL_FORMAT_FILES)
    message(
      WARNING
        "Could not find any source files to format. 'format' target will be empty."
    )
    return()
  endif()

  add_custom_target(
    format
    COMMAND ${ClangFormat_EXECUTABLE} -i ${ALL_FORMAT_FILES}
    COMMENT "Formatting project C/C++ files with clang-format..."
    VERBATIM
  )

  message(STATUS "Added 'format' target to run clang-format.")

endfunction()
