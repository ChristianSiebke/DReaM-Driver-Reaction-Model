set(COMPONENT_TEST_NAME AlgorithmAEB_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Algorithm_AEB)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    AlgorithmAeb_Tests.cpp
    BoundingBoxCalculation_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/algorithm_autonomousEmergencyBrakingImplementation.cpp
    ${COMPONENT_SOURCE_DIR}/boundingBoxCalculation.cpp

  HEADERS
    AlgorithmAebOSIUnitTests.h
    ${COMPONENT_SOURCE_DIR}/algorithm_autonomousEmergencyBrakingImplementation.h
    ${COMPONENT_SOURCE_DIR}/boundingBoxCalculation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

