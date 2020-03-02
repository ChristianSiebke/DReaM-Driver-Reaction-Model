#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES sensor_recordState.cpp
			 sensor_recordStateImplementation.cpp)
SET (HEADERS sensor_recordState.h
			 sensor_recordStateGlobal.h
			 sensor_recordStateImplementation.h)

add_definitions(-DSENSOR_RECORD_STATE_LIBRARY)

OPENPASS_add_component(Sensor_RecordState "")
