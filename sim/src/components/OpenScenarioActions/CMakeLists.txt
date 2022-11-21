################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME OpenScenarioActions)

add_compile_definitions(OPENSCENARIO_ACTIONS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    openScenarioActions.h
    src/oscActionsCalculation.h
    src/openScenarioActionsImpl.h
    src/oscActionsCalculation.h
    src/actionTransformRepository.h
    src/transformerBase.h
    src/transformTrajectory.h
    src/transformLaneChange.h
    src/transformSpeedAction.h
    src/transformAcquirePosition.h
    src/transformDefaultCustomCommandAction.h

  SOURCES
    openScenarioActions.cpp
    src/oscActionsCalculation.cpp
    src/openScenarioActionsImpl.cpp
    src/oscActionsCalculation.cpp
    src/transformLaneChange.cpp
    src/transformSpeedAction.cpp
    src/transformAcquirePosition.cpp
    src/transformDefaultCustomCommandAction.cpp

  LIBRARIES
    Qt5::Core
)
