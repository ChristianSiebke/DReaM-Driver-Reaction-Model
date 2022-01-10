################################################################################
# Copyright (c) 2020 HLRS, University of Stuttgart
#               2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(COMPONENT_NAME SpawnerScenario)

add_compile_definitions(SPAWNER_SCENARIO_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    ../../../framework/sampler.h
    ../../../modelElements/agentBlueprint.h
        SpawnerScenario.h
        SpawnerScenarioExport.h
        SpawnerScenarioGlobal.h

  SOURCES
    ../../../framework/sampler.cpp
    ../../../modelElements/agentBlueprint.cpp
        SpawnerScenario.cpp
        SpawnerScenarioExport.cpp

  INCDIRS
    ..
    ../../../
    ../../../modelElements/
    ../../../../common
    
  LIBRARIES
    Qt5::Core
    CoreCommon
)

