#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES agentUpdater.cpp
			 agentUpdaterImplementation.cpp)
set(HEADERS agentUpdater.h
			 agentUpdaterGlobal.h
			 agentUpdaterImplementation.h)

add_definitions(-DAGENT_UPDATER_LIBRARY)

OPENPASS_add_component(AgentUpdater "")
