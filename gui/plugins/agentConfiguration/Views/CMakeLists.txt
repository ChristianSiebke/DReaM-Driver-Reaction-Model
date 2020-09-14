# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateComboBoxView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateDoubleView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WidgetView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AgentMapView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SystemProfileView.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateComboBoxView.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/DelegateDoubleView.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/EditDataCommand.h
    ${CMAKE_CURRENT_LIST_DIR}/WidgetView.h
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationView.h
    ${CMAKE_CURRENT_LIST_DIR}/AgentMapView.h
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesView.h
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesView.h
    ${CMAKE_CURRENT_LIST_DIR}/SystemProfileView.h
    )

set(UI
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationView.ui
    ${CMAKE_CURRENT_LIST_DIR}/AgentMapView.ui
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesView.ui
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesView.ui
    ${CMAKE_CURRENT_LIST_DIR}/SystemProfileView.ui
    )

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Views PRIVATE Qt5::Widgets)
