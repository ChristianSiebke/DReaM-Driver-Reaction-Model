#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/
set(COMPONENT_NAME Window)

add_compile_definitions(Window_LIBRARY)

include_directories(.)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared LINKGUI COMPONENT gui

  HEADERS
    WindowPlugin.h 
	Models/WindowModel.h
	Views/WindowView.h
	Views/LogoDelegate.h
	Presenters/WindowPresenter.h
	Interfaces/openPASS-Window/WindowInterface.h

  SOURCES
    WindowPlugin.cpp
	Models/WindowModel.cpp
	Views/WindowView.cpp
	Views/LogoDelegate.cpp
	Presenters/WindowPresenter.cpp
	
  UIS 
	Views/WindowView.ui
	
  LIBRARIES
      openPASSInterfaces
)
