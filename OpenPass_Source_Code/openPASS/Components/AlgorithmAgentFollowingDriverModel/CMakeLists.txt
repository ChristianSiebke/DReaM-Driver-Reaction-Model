#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES AlgorithmAgentFollowingDriverModel.cpp
			 AlgorithmAgentFollowingDriverModelImplementation.cpp)
set(HEADERS AlgorithmAgentFollowingDriverModel.h
			 AlgorithmAgentFollowingDriverModelGlobal.h
			 AlgorithmAgentFollowingDriverModelImplementation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DALGORITHM_AGENTFOLLOWINGDRIVERMODEL_LIBRARY)

OPENPASS_add_component(AlgorithmAgentFollowingDriverModel "")
target_link_libraries(AlgorithmAgentFollowingDriverModel Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})
