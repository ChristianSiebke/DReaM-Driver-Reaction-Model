set(COMPONENT_TEST_NAME SensorDriver_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/Sensor_Driver)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    sensorDriver_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensor_driverCalculations.cpp
    ${COMPONENT_SOURCE_DIR}/sensor_driverImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensor_driverCalculations.h
    ${COMPONENT_SOURCE_DIR}/sensor_driverImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

