#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES objectDetectorBase.cpp
			 sensorGeometric2D.cpp
			 sensorObjectDetectorFactory.cpp)
set(HEADERS objectDetectorBase.h
			 sensorGeometric2D.h
			 sensorObjectDetectorFactory.h
			 sensorObjectDetectorGlobal.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

add_compile_definitions(SENSOR_OBJECT_DETECTOR_LIBRARY)

OPENPASS_add_component(Sensor_OSI "")

target_link_libraries(Sensor_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})