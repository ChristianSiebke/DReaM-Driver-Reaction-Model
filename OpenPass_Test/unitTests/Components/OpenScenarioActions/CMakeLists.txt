set(COMPONENT_TEST_NAME OpenScenarioActions_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/OpenScenarioActions)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    openScenarioActions_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/openScenarioActionsImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/openScenarioActionsImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

