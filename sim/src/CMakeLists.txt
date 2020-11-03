################################################################################
# Copyright (c) 2020 HLRS, University of Stuttgart
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(FOLDER "src")

include_directories(${CMAKE_CURRENT_LIST_DIR})

if(SIMCORE_VERSION_MAJOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_MINOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_PATCH MATCHES "^[0-9]+$")
  set(OPENPASS_VERSION "${SIMCORE_VERSION_MAJOR},${SIMCORE_VERSION_MINOR},${SIMCORE_VERSION_PATCH}")
elseif(DEFINED SIMCORE_VERSION_TAG)
  set(OPENPASS_VERSION "\"${SIMCORE_VERSION_TAG}\"")
else()
  set(OPENPASS_VERSION "\"undefined-version\"")
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/common/version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/common/version.h
)

add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(core)
