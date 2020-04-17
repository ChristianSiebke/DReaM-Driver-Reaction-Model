#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME SpawnPoint_OSI)

add_compile_definitions(SPAWNPOINT_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS SpawnPoint.h
          SpawnPointExport.h
          SpawnPointGlobal.h
          SpawnPointSceneryParser.h

  SOURCES
           SpawnPoint.cpp
           SpawnPointExport.cpp
           SpawnPointSceneryParser.cpp

  LIBRARIES
    Qt5::Core
)

