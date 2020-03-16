#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES dynamics_regularDriving.cpp
			 dynamics_regularDrivingImplementation.cpp)
set(HEADERS dynamics_regularDriving.h
			 dynamics_regularDrivingImplementation.h
			 dynamics_regularDrivingGlobal.h)

add_definitions(-DDYNAMICS_REGULAR_DRIVING_LIBRARY)

OPENPASS_add_component(Dynamics_RegularDriving "")
