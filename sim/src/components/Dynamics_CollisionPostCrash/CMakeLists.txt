#/*******************************************************************************
#* Copyright (c) 2020 ITK Engineering GmbH
#*               2020 in-tech
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME Dynamics_CollisionPostCrash)

add_compile_definitions(DYNAMICS_COLLISIONPOSTCRASH_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_collisionPostCrash.h
    src/collisionPostCrash.h

  SOURCES
    dynamics_collisionPostCrash.cpp
    src/collisionPostCrash.cpp

  LIBRARIES
    Qt5::Core
)
