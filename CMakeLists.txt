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
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/OpenPass_Source_Code)

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/OpenPass_Test)
endif()

include(CPack)

get_property(EXE_TARGET_LIST GLOBAL PROPERTY exe_target_list)
get_property(LIB_TARGET_LIST GLOBAL PROPERTY lib_target_list)

install(CODE
  "
  file(GET_RUNTIME_DEPENDENCIES
       RESOLVED_DEPENDENCIES_VAR resolved_deps
       UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
       EXECUTABLES ${EXE_TARGET_LIST}
       LIBRARIES ${LIB_TARGET_LIST})

  if(UNIX)
    list(FILTER resolved_deps EXCLUDE REGEX \"^/lib|^/usr\")
  endif()

  message(\"Installing deps: \${resolved_deps}\")
  message(\"Not found deps: \${unresolved_deps}\")

  file(COPY \${resolved_deps}
       DESTINATION ${CMAKE_INSTALL_PREFIX}
       FOLLOW_SYMLINK_CHAIN)
  "
)
