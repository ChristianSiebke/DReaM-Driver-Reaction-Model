################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

if(SIMCORE_VERSION_MAJOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_MINOR MATCHES "^[0-9]+$" AND
   SIMCORE_VERSION_PATCH MATCHES "^[0-9]+$")
  set(OPENPASS_VERSION "${SIMCORE_VERSION_MAJOR},${SIMCORE_VERSION_MINOR},${SIMCORE_VERSION_PATCH}")
elseif(DEFINED SIMCORE_VERSION_TAG)
  set(OPENPASS_VERSION "\"${SIMCORE_VERSION_TAG}\"")
else()
  set(OPENPASS_VERSION "9999,9999,9999")
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/version.h
)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/..
)

if(WITH_SIMCORE)
  add_subdirectory(src)
endif()

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(WITH_DOC)
  string(REPLACE "," "." OPENPASS_DOC_VERSION ${OPENPASS_VERSION})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/source/version.txt.in
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/source/version.txt
  )

  add_subdirectory(doc)
endif()
