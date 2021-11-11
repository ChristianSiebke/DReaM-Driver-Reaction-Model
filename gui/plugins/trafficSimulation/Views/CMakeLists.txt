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
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateComboBoxView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateDoubleView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateTimeView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DelegateFileBrowseView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FileBrowseView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WidgetView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/StatusBarView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentBaseView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioDisplayView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TrafficDisplayView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentDisplayView.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateComboBoxView.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateDoubleView.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateTimeView.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/EditDataCommand.h
    ${CMAKE_CURRENT_LIST_DIR}/DelegateFileBrowseView.h
    ${CMAKE_CURRENT_LIST_DIR}/FileBrowseView.h
    ${CMAKE_CURRENT_LIST_DIR}/WidgetView.h
    ${CMAKE_CURRENT_LIST_DIR}/StatusBarView.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationView.h
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentBaseView.h
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentView.h
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioView.h
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioDisplayView.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficView.h
    ${CMAKE_CURRENT_LIST_DIR}/TrafficDisplayView.h
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentView.h
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentDisplayView.h
    )

set(UI
    ${CMAKE_CURRENT_LIST_DIR}/FileBrowseView.ui
    ${CMAKE_CURRENT_LIST_DIR}/TrafficSimulationView.ui
    ${CMAKE_CURRENT_LIST_DIR}/StatusBarView.ui
    ${CMAKE_CURRENT_LIST_DIR}/ExperimentView.ui
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioView.ui
    ${CMAKE_CURRENT_LIST_DIR}/ScenarioDisplayView.ui
    ${CMAKE_CURRENT_LIST_DIR}/TrafficView.ui
    ${CMAKE_CURRENT_LIST_DIR}/TrafficDisplayView.ui
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentView.ui
    ${CMAKE_CURRENT_LIST_DIR}/EnvironmentDisplayView.ui

    )

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Views PRIVATE Qt5::Widgets)
