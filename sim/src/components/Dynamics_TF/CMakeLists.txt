set(COMPONENT_NAME Dynamics_TrajectoryFollower)

add_compile_definitions(DYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_tf.h
    src/tfImplementation.h

  SOURCES
    dynamics_tf.cpp
    src/tfImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common
    CoreCommon
)
