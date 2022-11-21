################################################################################
# Copyright (c) 2020 HLRS, University of Stuttgart
#               2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(FOLDER "src")

include_directories(${CMAKE_CURRENT_LIST_DIR}
                    ${CMAKE_CURRENT_LIST_DIR}/..)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/common/version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/common/version.h
)

add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(core)
add_subdirectory(tudresden)