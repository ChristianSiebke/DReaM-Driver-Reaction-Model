#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES dynamics_longitudinal_basic.cpp
			 dynamics_longitudinal_basic_implementation.cpp)
set(HEADERS dynamics_longitudinal_basic.h
			 dynamics_longitudinal_basic_implementation.h
			 dynamics_longitudinal_basic_global.h)

			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

add_definitions(-DDYNAMICS_LONGITUDINAL_BASIC_LIBRARY)

OPENPASS_add_component(Dynamics_Longitudinal_Basic "")

target_link_libraries(Dynamics_Longitudinal_Basic Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})