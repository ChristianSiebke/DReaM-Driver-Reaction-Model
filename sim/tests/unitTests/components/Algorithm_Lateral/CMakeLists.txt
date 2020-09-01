set(COMPONENT_TEST_NAME AlgorithmLateral_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_Lateral/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    algorithmLateral_Tests.cpp
    testResourceManager.cpp
    ${COMPONENT_SOURCE_DIR}/lateralImpl.cpp

  HEADERS
    testResourceManager.h
    testAlgorithmLateralImplementation.h
    ${COMPONENT_SOURCE_DIR}/lateralImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

