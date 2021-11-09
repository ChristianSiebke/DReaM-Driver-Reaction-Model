set(COMPONENT_TEST_NAME BasicDataBuffer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/BasicDataBuffer)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    basicDataBuffer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/basicDataBufferImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/basicDataBufferImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

