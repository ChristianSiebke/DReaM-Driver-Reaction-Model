#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES action_secondaryDriverTasks.cpp
			 action_secondaryDriverTasksImplementation.cpp)
SET (HEADERS action_secondaryDriverTasks.h
			 action_secondaryDriverTasksGlobal.h
			 action_secondaryDriverTasksImplementation.h)

add_definitions(-DACTION_SECONDARY_DRIVER_TASKS_LIBRARY)

OPENPASS_add_component(Action_SecondaryDriverTasks "")
