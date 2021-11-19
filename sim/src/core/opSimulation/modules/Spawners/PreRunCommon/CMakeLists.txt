################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(COMPONENT_NAME SpawnerPreRunCommon)

add_compile_definitions(SPAWNER_COMMON_PRERUN_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    ../../../framework/sampler.h
    ../../../modelElements/agentBlueprint.h
        ../common/SpawnerDefinitions.h
    ../common/WorldAnalyzer.h
        SpawnerPreRunCommon.h
        SpawnerPreRunCommonExport.h
        SpawnerPreRunCommonGlobal.h
        SpawnerPreRunCommonParameterExtractor.h

  SOURCES
    ../../../framework/sampler.cpp
    ../../../modelElements/agentBlueprint.cpp
    ../common/WorldAnalyzer.cpp
        SpawnerPreRunCommon.cpp
        SpawnerPreRunCommonExport.cpp

  INCDIRS
    ..
    ../../..
    ../../../../common
    ../../../modelElements

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreCommon
)
