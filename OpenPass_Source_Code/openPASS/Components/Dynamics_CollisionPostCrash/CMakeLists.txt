#/*******************************************************************************
#* Copyright (c) 2020 ITK Engineering GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME Dynamics_CollisionPostCrash)

add_compile_definitions(DYNAMICS_COLLISIONPOSTCRASH_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_collisionPostCrash.h
    dynamics_collisionPostCrashImplementation.h
    dynamics_collisionPostCrashGlobal.h

  SOURCES
    dynamics_collisionPostCrash.cpp
    dynamics_collisionPostCrashImplementation.cpp

  LIBRARIES
    Qt5::Core
)
