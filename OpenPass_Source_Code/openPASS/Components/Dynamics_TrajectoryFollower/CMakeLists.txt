#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES dynamics_trajectoryFollower.cpp
			 trajectoryFollowerCommonBase.cpp
			 roadCoordinateTrajectoryFollower.cpp
			 absoluteWorldCoordinateTrajectoryFollower.cpp
            ../../CoreFramework/CoreShare/log.cpp
            ../../CoreFramework/CoreShare/xmlParser.cpp
           ../../CoreFramework/OpenPassSlave/importer/csvParser.cpp
           ../../CoreFramework/OpenPassSlave/importer/trajectory.cpp
           ../../CoreFramework/OpenPassSlave/importer/trajectoryImporter.cpp
			 )
set(HEADERS dynamics_trajectoryFollower.h
			 trajectoryFollowerCommonBase.h
			 roadCoordinateTrajectoryFollower.h
			 dynamics_trajectoryFollowerGlobal.h
			 absoluteWorldCoordinateTrajectoryFollower.h)


add_definitions(-DDYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY)

OPENPASS_add_component(Dynamics_TrajectoryFollower "")
