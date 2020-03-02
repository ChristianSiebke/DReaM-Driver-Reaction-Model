#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES ProjectPlugin.cpp
Models/ProjectXMLSaveModel.cpp
Models/ProjectModel.cpp
Models/ProjectXMLLoadModel.cpp
Models/ProjectXMLBaseModel.cpp
Views/ProjectView.cpp
Presenters/ProjectPresenter.cpp
)
SET (HEADERS ProjectPlugin.h 
Models/ProjectModel.h
Models/ProjectXMLLoadModel.h
Models/ProjectXMLSaveModel.h
Models/ProjectXMLBaseModel.h
Views/ProjectView.h
Presenters/ProjectPresenter.h
Interfaces/openPASS-Project/ProjectInterface.h
)

SET(UIS 
Views/ProjectView.ui
)

INCLUDE_DIRECTORIES(.)

add_definitions(-DProject_LIBRARY)

OPENPASS_GUI_add_plugin(Project "")

target_link_libraries(Project openPASSInterfaces)

