# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

set(SOURCES
        SystemComponentInputItemModel.cpp
        SystemComponentInputMapModel.cpp
        SystemComponentItemModel.cpp
        SystemComponentManagerModel.cpp
        SystemComponentMapModel.cpp
        SystemComponentOutputItemModel.cpp
        SystemComponentOutputMapModel.cpp
        SystemComponentParameterItemModel.cpp
        SystemComponentParameterMapModel.cpp
        SystemComponentScheduleModel.cpp
        SystemConnectionItemModel.cpp
        SystemConnectionMapModel.cpp
        SystemItemModel.cpp
        SystemMapModel.cpp
        SystemModel.cpp
        SystemXMLBaseModel.cpp
        SystemXMLLoadModel.cpp
        SystemXMLSaveModel.cpp
   )

set(HEADERS
    SystemComponentInputItemModel.h
    SystemComponentInputMapModel.h
    SystemComponentItemModel.h
    SystemComponentManagerModel.h
    SystemComponentMapModel.h
    SystemComponentOutputItemModel.h
    SystemComponentOutputMapModel.h
    SystemComponentParameterItemModel.h
    SystemComponentParameterMapModel.h
    SystemComponentScheduleModel.h
    SystemConnectionItemModel.h
    SystemConnectionMapModel.h
    SystemItemModel.h
    SystemMapModel.h
    SystemModel.h
    SystemXMLBaseModel.h
    SystemXMLLoadModel.h
    SystemXMLSaveModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
add_definitions(-DSUBDIR_LIB_COMPONENTS="/components")
