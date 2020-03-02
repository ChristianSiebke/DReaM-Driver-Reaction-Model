set(COMPONENT_TEST_NAME DynamicsCollision_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Dynamics_Collision)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    dynamicsCollision_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/dynamics_collisionImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/dynamics_collisionImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

