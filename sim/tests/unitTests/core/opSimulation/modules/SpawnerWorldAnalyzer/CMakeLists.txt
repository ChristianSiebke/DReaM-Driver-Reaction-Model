################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME SpawnerWorldAnalyzer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Spawners)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnerWorldAnalyzer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/common/SpawnerDefinitions.h
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.h
    ${OPENPASS_SIMCORE_DIR}/common/RoutePlanning/RouteCalculation.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/framework/sampler.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation
    ${OPENPASS_SIMCORE_DIR}/common

  LIBRARIES
    Qt5::Core
    Common
    CoreCommon
)

