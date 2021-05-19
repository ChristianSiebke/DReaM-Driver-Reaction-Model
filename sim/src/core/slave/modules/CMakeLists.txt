#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(FOLDER "CoreModules")

add_subdirectory(BasicDataStore)
#add_subdirectory(CollisionDetection)
add_subdirectory(EventDetector)
add_subdirectory(Manipulator)
add_subdirectory(Observation_EntityRepository)
add_subdirectory(Observation_Log)
add_subdirectory(Observation_LogAgent)
#add_subdirectory(Observation_State)
#add_subdirectory(Observation_Ttc)
add_subdirectory(Spawners)
#add_subdirectory(SpawnPoint_Start)
add_subdirectory(Stochastics)
#add_subdirectory(World_Basic)
add_subdirectory(World_OSI)
