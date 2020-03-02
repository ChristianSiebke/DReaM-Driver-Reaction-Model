#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES WindowPlugin.cpp
Models/WindowModel.cpp
Views/WindowView.cpp
Views/LogoDelegate.cpp
Presenters/WindowPresenter.cpp
)
SET (HEADERS WindowPlugin.h 
Models/WindowModel.h
Views/WindowView.h
Views/LogoDelegate.h
Presenters/WindowPresenter.h
Interfaces/openPASS-Window/WindowInterface.h
)

SET(UIS 
Views/WindowView.ui
)

INCLUDE_DIRECTORIES(.)


add_definitions(-DWindow_LIBRARY)

OPENPASS_GUI_add_plugin(Window "")

target_link_libraries(Window openPASSInterfaces)