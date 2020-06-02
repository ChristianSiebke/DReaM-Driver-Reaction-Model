set(COMPONENT_TEST_NAME SensorFusionOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/SensorFusion_OSI)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    sensorFusionOSI_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensorFusionImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensorFusionImplementation.h
    ${COMPONENT_SOURCE_DIR}/sensorFusionQuery.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

