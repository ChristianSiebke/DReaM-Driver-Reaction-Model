set(COMPONENT_NAME SensorFusionErrorless_OSI)

add_compile_definitions(SENSOR_FUSION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensorFusionErrorless_OSI.h
    src/sensorFusionImpl.h

  SOURCES
    sensorFusionErrorless_OSI.cpp
    src/sensorFusionImpl.cpp

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
