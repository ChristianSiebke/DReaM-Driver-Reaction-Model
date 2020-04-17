#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/
set(COMPONENT_NAME Scenery)

add_compile_definitions(Scenery_LIBRARY)

include_directories(.)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared LINKGUI COMPONENT gui

  HEADERS
    SceneryPlugin.h 
	Models/SceneryModel.h
	Views/SceneryView.h
	Presenters/SceneryPresenter.h

  SOURCES
    SceneryPlugin.cpp
	Models/SceneryModel.cpp
	Views/SceneryView.cpp
	Presenters/SceneryPresenter.cpp
	
  UIS 
	Views/SceneryView.ui
	
  LIBRARIES
      openPASSInterfaces
	  Project
)
