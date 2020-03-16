#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES algorithm_autonomousEmergencyBraking.cpp
			 algorithm_autonomousEmergencyBrakingImplementation.cpp
			 boundingBoxCalculation.cpp)
set(HEADERS algorithm_autonomousEmergencyBraking.h
			 algorithm_autonomousEmergencyBrakingGlobal.h
			 algorithm_autonomousEmergencyBrakingImplementation.h
			 boundingBoxCalculation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})



add_definitions(-DALGORITHM_AEB_LIBRARY)

OPENPASS_add_component(Algorithm_AEB "")

target_link_libraries(Algorithm_AEB Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})