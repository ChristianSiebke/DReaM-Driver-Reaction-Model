#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES SceneryPlugin.cpp
Models/SceneryModel.cpp
Views/SceneryView.cpp
Presenters/SceneryPresenter.cpp
)
SET (HEADERS SceneryPlugin.h 
Models/SceneryModel.h
Views/SceneryView.h
Presenters/SceneryPresenter.h
)

SET(UIS 
Views/SceneryView.ui
)


INCLUDE_DIRECTORIES(.)

add_definitions(-DScenery_LIBRARY)

OPENPASS_GUI_add_plugin(Scenery "")

target_link_libraries(Scenery Project openPASSInterfaces)
