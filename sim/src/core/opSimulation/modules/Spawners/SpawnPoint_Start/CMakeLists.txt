#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#* Copyright (c) 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME Spawner_Start)

add_compile_definitions(SPAWNER_START_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
        spawner_start.h
        spawner_start_global.h
        spawner_start_implementation.h

  SOURCES
        spawner_start.cpp
        spawner_start_implementation.cpp

  LIBRARIES
    Qt5::Core
)

