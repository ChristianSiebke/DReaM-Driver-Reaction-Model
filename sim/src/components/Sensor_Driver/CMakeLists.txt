set(COMPONENT_NAME Sensor_Driver)

add_compile_definitions(SENSOR_DRIVER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensor_driver.h
    src/sensor_driverCalculations.h
    src/sensor_driverImpl.h
    src/Signals/sensorDriverSignal.h
    src/Signals/sensor_driverDefinitions.h

  SOURCES
    sensor_driver.cpp
    src/sensor_driverCalculations.cpp
    src/sensor_driverImpl.cpp

  INCDIRS
    src
    ../../core/slave/modules/World_OSI/RoutePlanning

  LIBRARIES
    Qt5::Core
)
