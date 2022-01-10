################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

##
# Set compiler flags for code coverage
#
# @author René Paris, in-tech GmbH
#
function(set_coverage_compiler_flags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage -O0" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage -O0" PARENT_SCOPE)
endfunction()

##
# Adds a depended test stage for collection of
# coverage information and coverage report generation
#
# ADD_TEST_COVERAGE_FASTCOV(NAME)
#   - NAME: Name of the test target, to which the coverage belongs
#
# @author René Paris, in-tech GmbH
#
function(add_test_coverage_fastcov)
    set(options, "")
    set(oneValueArgs NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(COVERAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(FASTCOV_COMMAND ${FASTCOV_EXECUTABLE}
        --gcov ${GCOV_EXECUTABLE}
        --search-directory ${CMAKE_CURRENT_BINARY_DIR}
        --process-gcno
        --lcov
        --output ${COVERAGE_NAME}.info
        --exclude ${COVERAGE_EXCLUDES}
    )

    set(GENHTML_COMMAND ${GENHTML_EXECUTABLE}
        --demangle-cpp
        -o ${CMAKE_CURRENT_BINARY_DIR}/html ${COVERAGE_NAME}.info
    )

    add_test(NAME ${COVERAGE_NAME}_coverage COMMAND ${FASTCOV_COMMAND})
    add_test(NAME ${COVERAGE_NAME}_html COMMAND ${GENHTML_COMMAND})
    set_tests_properties(${COVERAGE_NAME}_coverage PROPERTIES DEPENDS ${COVERAGE_NAME})
    set_tests_properties(${COVERAGE_NAME}_html PROPERTIES DEPENDS ${COVERAGE_NAME}_coverage)

endfunction()