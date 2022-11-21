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
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleProfileMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleProfileItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SensorMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SensorItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentProfileMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SensorProfileMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentProfileItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SensorProfileItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParameterMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParameterItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DriverProfileItemModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DriverProfileMapModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationXMLBaseModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationXMLSaveModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLBaseModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLSaveModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLLoadModel.cpp

   )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleProfileMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleProfileItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/SensorMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/SensorItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentProfileMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/SensorProfileMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/VehicleComponentProfileItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/SensorProfileItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/ParameterMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/ParameterItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/DriverProfileItemModel.h
        ${CMAKE_CURRENT_LIST_DIR}/DriverProfileMapModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationXMLBaseModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationXMLSaveModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLBaseModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLSaveModel.h
        ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationXMLLoadModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets)

