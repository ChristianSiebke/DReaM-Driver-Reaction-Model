#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES sensor_distance.cpp
			 sensor_distance_implementation.cpp)
SET (HEADERS sensor_distance.h
			 sensor_distance_implementation.h
			 sensor_distance_global.h)

add_definitions(-DSENSOR_DISTANCE_LIBRARY)

OPENPASS_add_component(Sensor_Distance "")
