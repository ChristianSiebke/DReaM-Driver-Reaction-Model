################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME Spawner_IntegrationTests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Spawners)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements/parameters.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings/modelLibrary.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.cpp
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.cpp
    ${COMPONENT_SOURCE_DIR}/PreRunCommon/SpawnerPreRunCommon.cpp
    Spawner_IntegrationTests.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.h
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.h
    ${COMPONENT_SOURCE_DIR}/PreRunCommon/SpawnerPreRunCommon.h

  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/bindings
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modelElements
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/observationInterface
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/spawnPointInterface
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/common
    ${COMPONENT_SOURCE_DIR}/PreRunCommon

  LIBRARIES
    Qt5::Core
    CoreCommon
)

