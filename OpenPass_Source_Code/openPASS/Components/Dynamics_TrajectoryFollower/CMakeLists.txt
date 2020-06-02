set(COMPONENT_NAME Dynamics_TrajectoryFollower)

add_compile_definitions(DYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_trajectoryFollower.h
    trajectoryFollowerCommonBase.h
    roadCoordinateTrajectoryFollower.h
    dynamics_trajectoryFollowerGlobal.h
    absoluteWorldCoordinateTrajectoryFollower.h

  SOURCES
    dynamics_trajectoryFollower.cpp
    trajectoryFollowerCommonBase.cpp
    roadCoordinateTrajectoryFollower.cpp
    absoluteWorldCoordinateTrajectoryFollower.cpp
    ../../CoreFramework/CoreShare/log.cpp
    ../../CoreFramework/CoreShare/xmlParser.cpp
    ../../CoreFramework/OpenPassSlave/importer/csvParser.cpp
    ../../CoreFramework/OpenPassSlave/importer/trajectory.cpp
    ../../CoreFramework/OpenPassSlave/importer/trajectoryImporter.cpp

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)
