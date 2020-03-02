#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES sensor_driver.cpp
			 sensor_driverCalculations.cpp
			 sensor_driverImplementation.cpp)
SET (HEADERS sensor_driver.h
			 sensor_driverCalculations.h
			 sensor_driverImplementation.h
			 sensor_driverGlobal.h
			 Signals/sensorDriverSignal.h
			 Signals/sensor_driverDefinitions.h)

add_definitions(-DSENSOR_DRIVER_LIBRARY)

OPENPASS_add_component(Sensor_Driver "")
