#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES observation_ttc.cpp
			 observation_ttc_implementation.cpp)
SET (HEADERS observation_ttc.h
			 observation_ttc_implementation.h
			 observation_ttc_global.h)


add_definitions(-DOBSERVATION_TTC_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(Observation_Ttc "")