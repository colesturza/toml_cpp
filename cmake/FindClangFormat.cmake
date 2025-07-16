# Find clang-format
#
# This module finds the clang-format executable. It provides OS-specific hints
# to locate the program in common installation directories on Windows (MSYS2),
# macOS (Homebrew), and Linux.
#
# This module will define the following variables:
#
# * ClangFormat_FOUND      - True if the clang-format executable was found.
# * ClangFormat_EXECUTABLE - The path to the clang-format executable.

set(CLANG_FORMAT_HINTS)
if(WIN32)
  set(CLANG_FORMAT_HINTS "C:/msys64/ucrt64/bin" "C:/msys64/mingw64/bin"
                         "C:/msys64/clang64/bin" "C:/msys64/usr/bin"
  )
elseif(APPLE)
  set(CLANG_FORMAT_HINTS "/opt/homebrew/bin" "/usr/local/opt/llvm/bin"
                         "/usr/local/bin" "/usr/bin"
  )
elseif(UNIX)
  set(CLANG_FORMAT_HINTS "/usr/local/bin" "/usr/bin")
endif()

find_program(
  ClangFormat_EXECUTABLE
  NAMES clang-format-15
        clang-format-14
        clang-format-13
        clang-format-12
        clang-format-11
        clang-format-10
        clang-format
  HINTS ${CLANG_FORMAT_HINTS}
  DOC "Path to the clang-format executable"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  ClangFormat REQUIRED_VARS ClangFormat_EXECUTABLE
)

if(ClangFormat_FOUND)
  message(STATUS "Found clang-format: ${ClangFormat_EXECUTABLE}")
else()
  message(
    WARNING
      "clang-format not found. Please install it or add it to your system's PATH."
  )
endif()

mark_as_advanced(ClangFormat_EXECUTABLE)
