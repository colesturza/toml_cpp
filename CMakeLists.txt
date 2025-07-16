cmake_minimum_required(VERSION 3.31)
project(
  toml_cpp
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ClangFormatProject)
add_clang_format_target(
  DIRECTORIES ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests
)

add_library(toml_cpp SHARED src/toml_cpp/lexer.cc src/toml_cpp/parser.cc)
add_library(toml_cpp::toml_cpp ALIAS toml_cpp)

target_include_directories(
  toml_cpp
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

if(WIN32)
  set_target_properties(toml_cpp PROPERTIES PREFIX "lib")
endif()

# ===================================================================
# Testing Section 🧪
# ===================================================================
option(TOML_CPP_BUILD_TESTS "Build the tests for toml_cpp" ON)

if(TOML_CPP_BUILD_TESTS)
  enable_testing()

  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )

  FetchContent_MakeAvailable(googletest)

  add_executable(
    toml_cpp_tests tests/test_lexer.cc tests/test_parser.cc
                   tests/test_local_time.cc tests/test_local_date.cc
  )

  target_link_libraries(toml_cpp_tests PRIVATE toml_cpp GTest::gtest_main)

  target_include_directories(
    toml_cpp_tests
    PRIVATE ${googletest_SOURCE_DIR}/googletest/include
            ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tests
  )

  include(GoogleTest)
  gtest_discover_tests(toml_cpp_tests)
endif()
# ===================================================================

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

install(
  TARGETS toml_cpp
  EXPORT ${TARGETS_EXPORT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT ${TARGETS_EXPORT_NAME}
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  "cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
