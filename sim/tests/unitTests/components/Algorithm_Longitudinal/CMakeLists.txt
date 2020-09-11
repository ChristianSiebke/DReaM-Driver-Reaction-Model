set(COMPONENT_TEST_NAME AlgorithmLongitudinal_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_Longitudinal/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    algorithmLongitudinal_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/longCalcs.cpp
    ${COMPONENT_SOURCE_DIR}/algo_longImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/longCalcs.h
    ${COMPONENT_SOURCE_DIR}/algo_longImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

