#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES algorithm_lateral.cpp
			 algorithm_lateralImplementation.cpp)
set(HEADERS algorithm_lateral.h
			 algorithm_lateralGlobal.h
			 algorithm_lateralImplementation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DALGORITHM_LATERAL_LIBRARY)

OPENPASS_add_component(Algorithm_Lateral "")

target_link_libraries(Algorithm_Lateral Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})