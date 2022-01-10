################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME OpenScenarioActions_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/OpenScenarioActions/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    openScenarioActions_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/openScenarioActionsImpl.cpp
    ${COMPONENT_SOURCE_DIR}/oscActionsCalculation.cpp
    ${COMPONENT_SOURCE_DIR}/transformLaneChange.cpp
    ${COMPONENT_SOURCE_DIR}/transformSpeedAction.cpp
    ${COMPONENT_SOURCE_DIR}/transformAcquirePosition.cpp
    ${COMPONENT_SOURCE_DIR}/transformDefaultCustomCommandAction.cpp
    ${COMPONENT_SOURCE_DIR}/transformSpeedAction.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/openScenarioActionsImpl.h
    ${COMPONENT_SOURCE_DIR}/oscActionsCalculation.h
    ${OPENPASS_SIMCORE_DIR}/common/events/laneChangeEvent.h
    ${OPENPASS_SIMCORE_DIR}/common/events/trajectoryEvent.h
    ${OPENPASS_SIMCORE_DIR}/common/trajectorySignal.h
    ${OPENPASS_SIMCORE_DIR}/common/acquirePositionSignal.h
    ${COMPONENT_SOURCE_DIR}/transformDefaultCustomCommandAction.h
    ${OPENPASS_SIMCORE_DIR}/common/speedActionSignal.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

