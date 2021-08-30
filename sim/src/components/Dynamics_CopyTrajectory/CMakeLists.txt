set(COMPONENT_NAME Dynamics_CopyTrajectory)

add_compile_definitions(DYNAMICS_COPYTRAJECTORY_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_copyTrajectory.h
    dynamics_copyTrajectory_implementation.h
    dynamics_copyTrajectory_global.h

  SOURCES
    dynamics_copyTrajectory.cpp
    dynamics_copyTrajectory_implementation.cpp

  LIBRARIES
    Qt5::Core
)
