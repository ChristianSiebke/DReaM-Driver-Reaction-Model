set(COMPONENT_TEST_NAME SensorFusionOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/SensorFusion_OSI/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    sensorFusionOSI_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensorFusionImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensorFusionImpl.h
    ${COMPONENT_SOURCE_DIR}/sensorFusionQuery.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

