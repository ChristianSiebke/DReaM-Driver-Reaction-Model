#/*******************************************************************************
#* Copyright (c) 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME SpawnPointPreRunCommon_OSI)

add_compile_definitions(SPAWNPOINT_COMMON_PRERUN_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h
    ../Common/SpawnPointDefinitions.h
    ../Common/WorldAnalyzer.h
    SpawnPointPreRunCommon.h
    SpawnPointPreRunCommonExport.h
    SpawnPointPreRunCommonGlobal.h
    SpawnPointPreRunCommonParameterExtractor.h

  SOURCES
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    ../Common/WorldAnalyzer.cpp
    SpawnPointPreRunCommon.cpp
    SpawnPointPreRunCommonExport.cpp
    SpawnPointPreRunCommonParameterExtractor.cpp

  INCDIRS
    ..
    ../../../CoreFramework/OpenPassSlave/modelElements

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreShare
)
