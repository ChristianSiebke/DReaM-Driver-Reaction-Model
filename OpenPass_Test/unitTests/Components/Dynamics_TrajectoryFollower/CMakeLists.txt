set(COMPONENT_TEST_NAME DynamicsTrajectoryFollower_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Dynamics_TrajectoryFollower)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    dynamicsTrajectoryFollower_Tests.cpp
    trajectoryTester.cpp
    ${COMPONENT_SOURCE_DIR}/trajectoryFollowerImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/trajectoryFollowerImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Common
)

