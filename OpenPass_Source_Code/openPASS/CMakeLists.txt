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

set(FOLDER "Core")

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/Interfaces)

add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(CoreFramework)
add_subdirectory(CoreModules)

if(BUILD_PCM)
    add_subdirectory(Common_PCM)
    add_subdirectory(Components_PCM)
    add_subdirectory(CoreModules_PCM)
endif()
