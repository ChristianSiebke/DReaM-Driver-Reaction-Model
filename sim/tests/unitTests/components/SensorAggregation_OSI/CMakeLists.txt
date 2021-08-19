set(COMPONENT_TEST_NAME SensorAggregationOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/SensorAggregation_OSI/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    sensorAggregationOSI_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensorAggregationImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensorAggregationImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

