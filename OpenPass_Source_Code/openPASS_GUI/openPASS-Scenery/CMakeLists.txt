#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES SceneryPlugin.cpp
Models/SceneryModel.cpp
Views/SceneryView.cpp
Presenters/SceneryPresenter.cpp
)
set(HEADERS SceneryPlugin.h 
Models/SceneryModel.h
Views/SceneryView.h
Presenters/SceneryPresenter.h
)

set(UIS 
Views/SceneryView.ui
)


include_directories(.)

add_definitions(-DScenery_LIBRARY)

openpass_gui_add_plugin(Scenery "")

target_link_libraries(Scenery Project openPASSInterfaces)
