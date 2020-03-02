set(COMPONENT_TEST_NAME DynamicsTrajectoryFollower_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Dynamics_TF)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    dynamicsTF_Tests.cpp
    trajectoryTester.cpp
    ${COMPONENT_SOURCE_DIR}/tfImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/tfImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Common
)
