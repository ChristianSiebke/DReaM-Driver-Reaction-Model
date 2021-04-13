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

# used for collecting a list of targets for dependency resolving and installation at the bottom of this file
# these have to be set before creating targets
set_property(GLOBAL PROPERTY exe_target_list)
set_property(GLOBAL PROPERTY lib_target_list)
cmake_policy(SET CMP0087 NEW)   # Install CODE|SCRIPT allow the use of generator expressions.

include(global)

set(CPACK_PACKAGE_NAME openPASS)
set(CPACK_PACKAGE_VENDOR "simopenpass Eclipse project team")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 7)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_CHECKSUM "SHA1")
set(CPACK_STRIP_FILES TRUE)

# descend to sources
if(WITH_SIMCORE)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sim)
endif()

if(WITH_GUI)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gui)
endif()

if(WITH_TESTS)
  enable_testing()
endif()

# copies runtime dependencies to installation directory (on running 'make install')
# requires global properties defined above
# only has an effect, if at least one of INSTALL_SYSTEM_RUNTIME_DEPS and INSTALL_EXTRA_RUNTIME_DEPS is set to ON
include(install_deps)

include(CPack)
