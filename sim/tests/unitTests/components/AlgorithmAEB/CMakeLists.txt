set(COMPONENT_TEST_NAME AlgorithmAEB_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_AEB/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    AlgorithmAeb_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/autonomousEmergencyBraking.cpp

  HEADERS
    AlgorithmAebOSIUnitTests.h
    ${COMPONENT_SOURCE_DIR}/autonomousEmergencyBraking.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SIMCORE_DIR}/core/slave/modules/World_OSI

  LIBRARIES
    Qt5::Core
)

