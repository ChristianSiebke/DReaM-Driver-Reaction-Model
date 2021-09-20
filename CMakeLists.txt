################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# be prepared for support of C++20
cmake_minimum_required(VERSION 3.14)

project(openPASS C CXX)

if(WIN32 AND NOT MSYS)
  message(WARNING "It seems you are using a build environment different from MSYS. This is not officially supported by the openPASS project.")
endif()

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

if(SIMCORE_VERSION_MAJOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_MINOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_PATCH MATCHES "^[0-9]+$")
  set(OPENPASS_VERSION "${SIMCORE_VERSION_MAJOR},${SIMCORE_VERSION_MINOR},${SIMCORE_VERSION_PATCH}")
elseif(DEFINED SIMCORE_VERSION_TAG)
  set(OPENPASS_VERSION "\"${SIMCORE_VERSION_TAG}\"")
else()
  set(OPENPASS_VERSION "9999,9999,9999")
endif()

include_directories(.)

# descend to sources
if(WITH_SIMCORE)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sim/src)
endif()

if(WITH_GUI)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gui)
endif()

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sim/tests)
endif()

if(WITH_DOC OR WITH_API_DOC)
  string(REPLACE "," "." OPENPASS_DOC_VERSION ${OPENPASS_VERSION})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/source/version.txt.in
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/source/version.txt
  )

  add_subdirectory(doc)
endif()

# copies runtime dependencies to installation directory (on running 'make install')
# requires global properties defined above
if(INSTALL_SYSTEM_RUNTIME_DEPS OR INSTALL_EXTRA_RUNTIME_DEPS)
  include(install_deps)
endif()

include(CPack)
