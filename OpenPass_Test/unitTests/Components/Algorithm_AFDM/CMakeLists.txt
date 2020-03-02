set(COMPONENT_TEST_NAME AlgorithmAFDM_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/AlgorithmAFDM)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    Afdm_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/AlgorithmAFDMImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/AlgorithmAFDMImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/Components/Sensor_Driver/Signals

  LIBRARIES
    Qt5::Core
)

