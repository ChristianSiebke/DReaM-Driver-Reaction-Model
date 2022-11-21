################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(COMPONENT_TEST_NAME SpawnerPreRunCommon_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Spawners/PreRunCommon)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnerPreRunCommon_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/SpawnerPreRunCommon.cpp
    ${COMPONENT_SOURCE_DIR}/../common/WorldAnalyzer.cpp
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/agentBlueprintProvider.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicProfileSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicParametersSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicAgentTypeGenerator.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agentType.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/componentType.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/parameters.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelLibrary.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/SpawnerPreRunCommon.h
    ${COMPONENT_SOURCE_DIR}/../common/WorldAnalyzer.h
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/agentBlueprintProvider.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/agentDataPublisher.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicProfileSampler.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicParametersSampler.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/dynamicAgentTypeGenerator.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agent.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agentBlueprint.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agentType.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/channel.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/component.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/componentType.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/parameters.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelBinding.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelLibrary.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/..
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/importer
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements

  LIBRARIES
    Qt5::Core
    CoreCommon
)
