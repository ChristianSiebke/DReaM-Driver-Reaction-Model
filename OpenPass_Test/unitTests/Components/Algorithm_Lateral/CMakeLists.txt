set(COMPONENT_TEST_NAME AlgorithmLateral_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Algorithm_Lateral)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    algorithmLateral_Tests.cpp
    testResourceManager.cpp
    ${COMPONENT_SOURCE_DIR}/algorithm_lateralImplementation.cpp

  HEADERS
    testResourceManager.h
    testAlgorithmLateralImplementation.h
    ${COMPONENT_SOURCE_DIR}/algorithm_lateralImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

