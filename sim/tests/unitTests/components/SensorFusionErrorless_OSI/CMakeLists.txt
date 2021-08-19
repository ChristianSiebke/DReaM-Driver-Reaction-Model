set(COMPONENT_TEST_NAME SensorFusionErrorless_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/SensorFusionErrorless_OSI/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    sensorFusionErrorless_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensorFusionImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensorFusionImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core

  LINKOSI
)
