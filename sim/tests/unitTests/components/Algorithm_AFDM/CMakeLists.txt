set(COMPONENT_TEST_NAME AlgorithmAFDM_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/AlgorithmAFDM/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    Afdm_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/followingDriverModel.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/followingDriverModel.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/components/Sensor_Driver/src/Signals

  LIBRARIES
    Qt5::Core
)

