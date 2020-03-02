set(COMPONENT_NAME Dynamics_TrajectoryFollower)

add_compile_definitions(DYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    dynamics_trajectoryFollower.h
    trajectoryFollowerImplementation.h

  SOURCES
    dynamics_trajectoryFollower.cpp
    trajectoryFollowerImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)
