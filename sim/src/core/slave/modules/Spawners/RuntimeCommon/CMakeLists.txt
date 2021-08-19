#/*******************************************************************************
#* Copyright (c) 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME SpawnPointRuntimeCommon)

add_compile_definitions(SPAWNPOINT_COMMON_RUNTIME_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    ../../../framework/sampler.h
    ../../../modelElements/agentBlueprint.h
    ../common/SpawnPointDefinitions.h
    ../common/WorldAnalyzer.h
    SpawnPointRuntimeCommon.h
    SpawnPointRuntimeCommonExport.h
    SpawnPointRuntimeCommonGlobal.h
    SpawnPointRuntimeCommonParameterExtractor.h

  SOURCES
    ../../../framework/sampler.cpp
    ../../../modelElements/agentBlueprint.cpp
    ../common/WorldAnalyzer.cpp
    SpawnPointRuntimeCommon.cpp
    SpawnPointRuntimeCommonExport.cpp

  INCDIRS
    ..
    ../../..
    ../../../../common
    ../../../importer
    ../../../modelElements

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreCommon
)
