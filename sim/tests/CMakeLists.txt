################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(OPENPASS_SIMCORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
set(TEST_PATH ${CMAKE_CURRENT_LIST_DIR})

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${TEST_PATH}
  ${TEST_PATH}/common
  ${TEST_PATH}/common/gtest
  ${TEST_PATH}/fakes/gmock
  ${OPENPASS_SIMCORE_DIR}
)

add_compile_definitions(NODLL)

add_subdirectory(unitTests)
add_subdirectory(integrationTests)

if(WITH_ENDTOEND_TESTS)
  add_subdirectory(endToEndTests)
endif()
