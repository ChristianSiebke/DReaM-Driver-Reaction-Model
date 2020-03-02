set(COMPONENT_TEST_NAME AlgorithmLongitudinal_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Algorithm_Longitudinal)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    algorithmLongitudinal_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/algorithm_longitudinalCalculations.cpp
    ${COMPONENT_SOURCE_DIR}/algorithm_longitudinalImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/algorithm_longitudinalCalculations.h
    ${COMPONENT_SOURCE_DIR}/algorithm_longitudinalImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

