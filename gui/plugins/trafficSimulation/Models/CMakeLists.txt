################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLSaveModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLBaseModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLLoadModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficItemModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficItemMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentItemModel.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLSaveModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLBaseModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationXMLLoadModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficItemModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficItemMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficModel.h
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentModel.h
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentItemModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
