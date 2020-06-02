################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# be prepared for support of C++20
cmake_minimum_required(VERSION 3.13)

project(openPASS C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

include(global)

set(CPACK_PACKAGE_NAME openPASS)
set(CPACK_PACKAGE_VENDOR "simopenpass Eclipse project team")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 7)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_CHECKSUM "SHA1")
set(CPACK_STRIP_FILES TRUE)

# descend to sources
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/OpenPass_Source_Code)

if(WITH_TESTS)
  enable_testing()
  # will be provided in the near future
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/OpenPass_Test)
endif()

include(CPack)
