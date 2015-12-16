# Minimum version 
cmake_minimum_required(VERSION 3.1)

# Project definition
project(ddtable VERSION 1.0 LANGUAGES C)

# Options, mostly for testing purposes
option(BUILD_TESTS "Builds unit tests" ON)
option(BUILD_DOXYDOC "Build Doxygen documentation with target 'doc'" ON)

# Set output directories to avoid subdir hell on Windows
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

# Generate compiler detection header, used to test compiler support and
# defines workarounds as necessary.
include(WriteCompilerDetectionHeader)
write_compiler_detection_header(
  FILE "${CMAKE_CURRENT_BINARY_DIR}/config/ddtable_compiler_detection.h"
  PREFIX ddtable
  COMPILERS GNU Clang AppleClang MSVC
  FEATURES
  c_function_prototypes c_restrict
  )

# Generate take10_config.h, a general-purpose configuration file.
configure_file(ddtable_config.h.in config/ddtable_config.h @ONLY)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/config")

# Global flag used to facilitate the use of static analysis tools
add_definitions(-D__CMAKE__)
# Flag to indicate presence of ddtable_config.h file
add_definitions(-DHAVE_DDTABLE_CONFIG_H)

# Grab the source files
aux_source_directory(src/ SOURCE_FILES)

# Make a static library out of the source files
add_library(ddtablelib STATIC ${SOURCE_FILES})
target_include_directories(ddtablelib PUBLIC src)
set_property(TARGET ddtablelib PROPERTY C_STANDARD 99)

# Check whether building Debug vs Release build
if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
  message(STATUS "Building release, no debugging symbols or flags")
  set(DEBUG_BUILD OFF)
else()  # Release with debug, debug, or unspecified
  message(STATUS "Building with debugging symbols and flags")
  set(DEBUG_BUILD ON)
  # Turn on warnings when debugging
  if(CMAKE_COMPILER_IS_GNUC)
    target_compile_options(ddtablelib PRIVATE -Wall -Wpedantic -Wextra)
  elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(ddtablelib PRIVATE -Wall -Wpedantic -Wextra)
  elseif(MSVC)
    target_compile_options(ddtablelib PRIVATE \W4)
  endif()
endif()

# Build tests
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
  if(WIN32)
    ADD_CUSTOM_COMMAND(
      TARGET ddtablelib
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/ddtablelib.dll
      ${CMAKE_BINARY_DIR}/test/)
  endif(WIN32)
endif(BUILD_TESTS)

# (Optional) Generate API documentation with Doxygen ($make doc)
if(BUILD_DOXYDOC)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM)
  endif(DOXYGEN_FOUND)
endif(BUILD_DOXYDOC)
