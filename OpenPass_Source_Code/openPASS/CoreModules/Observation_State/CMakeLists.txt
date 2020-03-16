#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES observation_state.cpp
			 observation_state_implementation.cpp)
set(HEADERS observation_state.h
			 observation_state_implementation.h
			 observation_state_global.h
			 observationtypes.h)


add_definitions(-DOBSERVATION_STATE_LIBRARY)

OPENPASS_ADD_MODULE(Observation_State "")
