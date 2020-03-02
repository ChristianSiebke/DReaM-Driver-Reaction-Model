#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES limiterAccelerationVehicleComponents.cpp
			 limiterAccelerationVehicleComponentsImplementation.cpp)
SET (HEADERS limiterAccelerationVehicleComponents.h
			 limiterAccelerationVehicleComponentsImplementation.h
			 limiterAccelerationVehicleComponentsGlobal.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DLIMITER_ACCELERATION_VEHICLE_COMPONENTS_LIBRARY)

OPENPASS_add_component(LimiterAccelerationVehicleComponents "")

target_link_libraries(LimiterAccelerationVehicleComponents Common ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})