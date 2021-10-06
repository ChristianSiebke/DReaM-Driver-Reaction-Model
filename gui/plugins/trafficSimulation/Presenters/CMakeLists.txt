################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/ExperimentPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/ScenarioPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/ScenarioAgentsPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/ScenarioDisplayPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/TrafficPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/TrafficDisplayPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/TrafficItemMapPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/EnvironmentDisplayPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/EnvironmentMapPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/EnvironmentPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/LoggingGroupsPresenter.h
       ${CMAKE_CURRENT_LIST_DIR}/LibrariesPresenter.h
    )

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ExperimentPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ScenarioPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ScenarioAgentsPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ScenarioDisplayPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/TrafficPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/TrafficItemMapPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/TrafficDisplayPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EnvironmentDisplayPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EnvironmentMapPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EnvironmentPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LoggingGroupsPresenter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LibrariesPresenter.cpp
    )


add_library(${PROJECT_NAME}_Presenters STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Presenters PRIVATE ${PROJECT_NAME}_Models ${PROJECT_NAME}_Views)
