set(COMPONENT_TEST_NAME EventDetector_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreModules/EventDetector)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    CollisionDetectorUnitTests.cpp
    ConditionalEventDetector_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/CollisionDetector.cpp
    ${COMPONENT_SOURCE_DIR}/ConditionalEventDetector.cpp
    ${COMPONENT_SOURCE_DIR}/EventDetectorCommonBase.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/CollisionDetector.h
    ${COMPONENT_SOURCE_DIR}/ConditionalEventDetector.h
    ${COMPONENT_SOURCE_DIR}/EventDetectorCommonBase.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)

