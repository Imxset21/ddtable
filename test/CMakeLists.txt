# Add test executables
add_executable(test_ddtable test_ddtable.c)
set_property(TARGET test_ddtable PROPERTY C_STANDARD 99)
target_link_libraries(test_ddtable ddtablelib)

# Add tests
add_test(test_ddtable_test
  test_ddtable
  WORKING_DIRECTORY tests)

# Do coverage with kcov, if available: $make kcov
find_program(KCOV_EXECUTABLE NAMES kcov)
if(KCOV_EXECUTABLE)
  message(STATUS "kcov executable found: ${KCOV_EXECUTABLE}")
  # Set excludes to exclude common library locations, helper libraries, and tests themselves
  set(KCOV_EXCLUDE_PATTERN /usr/include,/usr/local/include,/usr/lib,test_)
  set(KCOV_RESULT_DIR ${CMAKE_BINARY_DIR}/coverage)
  # Add custom kcoverage target
  add_custom_target(kcoverage
    COMMAND kcov ${KCOV_RESULT_DIR} --exclude-pattern=${KCOV_EXCLUDE_PATTERN} test_ddtable
    DEPENDS test_ddtable
    COMMENT "Getting test coverage using kcov"
    VERBATIM)
else()
  message(WARNING "kcov executable not found, not calculating test coverage")
endif(KCOV_EXECUTABLE)
