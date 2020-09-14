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
    ${CMAKE_CURRENT_LIST_DIR}/ProjectModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLBaseModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLSaveModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLLoadModel.cpp

    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ProjectModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLBaseModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLSaveModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ProjectXMLLoadModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
