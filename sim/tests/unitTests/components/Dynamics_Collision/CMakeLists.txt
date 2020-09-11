set(COMPONENT_TEST_NAME DynamicsCollision_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Dynamics_Collision/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    dynamicsCollision_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/collisionImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/collisionImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

