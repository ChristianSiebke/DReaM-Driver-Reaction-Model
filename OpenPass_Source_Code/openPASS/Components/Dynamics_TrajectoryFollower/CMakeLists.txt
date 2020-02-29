SET (SOURCES dynamics_trajectoryFollower.cpp
			 trajectoryFollowerCommonBase.cpp
			 roadCoordinateTrajectoryFollower.cpp
			 absoluteWorldCoordinateTrajectoryFollower.cpp
            ../../CoreFramework/CoreShare/log.cpp
            ../../CoreFramework/CoreShare/xmlParser.cpp
           ../../CoreFramework/OpenPassSlave/importer/csvParser.cpp
           ../../CoreFramework/OpenPassSlave/importer/trajectory.cpp
           ../../CoreFramework/OpenPassSlave/importer/trajectoryImporter.cpp
			 )
SET (HEADERS dynamics_trajectoryFollower.h
			 trajectoryFollowerCommonBase.h
			 roadCoordinateTrajectoryFollower.h
			 dynamics_trajectoryFollowerGlobal.h
			 absoluteWorldCoordinateTrajectoryFollower.h)


add_definitions(-DDYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY)

OPENPASS_add_component(Dynamics_TrajectoryFollower "")
