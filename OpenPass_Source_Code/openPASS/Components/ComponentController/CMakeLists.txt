#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES componentController.cpp
			 componentControllerImplementation.cpp
			 componentStateInformation.cpp
			 condition.cpp
			 stateManager.cpp)
set(HEADERS componentController.h
			 componentControllerCommon.h
			 componentControllerGlobal.h
			 componentControllerImplementation.h
			 componentStateInformation.h
			 condition.h
			 stateManager.h)

add_compile_definitions(COMPONENT_CONTROLLER_LIBRARY)

OPENPASS_add_component(ComponentController "")
