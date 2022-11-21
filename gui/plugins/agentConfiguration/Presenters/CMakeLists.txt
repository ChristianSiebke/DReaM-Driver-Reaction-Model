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
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/AgentMapPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesMapPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesMapPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/ParameterMapPresenter.h
    ${CMAKE_CURRENT_LIST_DIR}/SystemProfilePresenter.h
    )

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AgentMapPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AgentConfigurationCombinationPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/VehicleProfilesMapPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DriverProfilesMapPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ParameterMapPresenter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SystemProfilePresenter.cpp
    )


add_library(${PROJECT_NAME}_Presenters STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Presenters PRIVATE ${PROJECT_NAME}_Models)
