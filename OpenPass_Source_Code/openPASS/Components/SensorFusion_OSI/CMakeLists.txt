#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES sensorFusion.cpp
			 sensorFusionImplementation.cpp)
SET (HEADERS sensorFusion.h
			 sensorFusionGlobal.h
			 sensorFusionImplementation.h
			 sensorFusionQuery.h)

add_definitions(-DSENSOR_FUSION_LIBRARY)

include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})


OPENPASS_add_component(SensorFusion_OSI OSI)

target_link_libraries(SensorFusion_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})