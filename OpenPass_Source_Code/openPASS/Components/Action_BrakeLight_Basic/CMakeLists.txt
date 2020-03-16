#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES action_brakelight_basic.cpp
			 action_brakelight_basic_implementation.cpp)
set(HEADERS action_brakelight_basic.h
			 action_brakelight_basic_implementation.h
			 action_brakelight_basic_global.h)


add_definitions(-DACTION_BRAKELIGHT_BASIC_LIBRARY)

OPENPASS_add_component(Action_BrakeLight_Basic "")