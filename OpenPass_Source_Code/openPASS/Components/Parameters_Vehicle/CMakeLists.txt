#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES parameters_vehicle.cpp
			 parameters_vehicleImplementation.cpp)
set(HEADERS parameters_vehicle.h
			 parameters_vehicleGlobal.h
			 parameters_vehicleImplementation.h)

add_definitions(-DPARAMETERS_VEHICLE_LIBRARY)

OPENPASS_add_component(Parameters_Vehicle "")
