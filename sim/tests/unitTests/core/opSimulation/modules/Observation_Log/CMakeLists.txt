################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME ObservationLog_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Observation_Log)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    observationLog_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/observation_logImplementation.cpp
    ${COMPONENT_SOURCE_DIR}/observationCyclics.cpp
    ${COMPONENT_SOURCE_DIR}/observationFileHandler.cpp
    ${COMPONENT_SOURCE_DIR}/runStatistic.cpp
    ${COMPONENT_SOURCE_DIR}/runStatisticCalculation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/observation_logImplementation.h
    ${COMPONENT_SOURCE_DIR}/observationCyclics.h
    ${COMPONENT_SOURCE_DIR}/observationFileHandler.h
    ${COMPONENT_SOURCE_DIR}/runStatistic.h
    ${COMPONENT_SOURCE_DIR}/runStatisticCalculation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

