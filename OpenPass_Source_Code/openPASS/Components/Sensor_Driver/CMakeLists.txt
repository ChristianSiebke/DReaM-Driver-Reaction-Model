set(COMPONENT_NAME Sensor_Driver)

add_compile_definitions(SENSOR_DRIVER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    sensor_driver.h
    sensor_driverCalculations.h
    sensor_driverImplementation.h
    sensor_driverGlobal.h
    Signals/sensorDriverSignal.h
    Signals/sensor_driverDefinitions.h

  SOURCES
    sensor_driver.cpp
    sensor_driverCalculations.cpp
    sensor_driverImplementation.cpp

  LIBRARIES
    Qt5::Core
)
