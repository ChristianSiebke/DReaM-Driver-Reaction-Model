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
    ${CMAKE_CURRENT_LIST_DIR}/ComponentInputItemModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentInputMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentItemModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentOutputItemModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentOutputMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentParameterItemModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentParameterMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentScheduleModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLBaseModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLLoadModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLSaveModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentTypedefMapModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ComponentTypedefItemModel.cpp

    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ComponentInputItemModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentInputMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentItemModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentOutputItemModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentOutputMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentParameterItemModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentParameterMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentScheduleModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLBaseModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLLoadModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentXMLSaveModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentTypedefMapModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ComponentTypedefItemModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets)
