set(COMPONENT_NAME SensorAggregation_OSI)

add_compile_definitions(SENSOR_AGGREGATION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensorAggregation.h
    src/sensorAggregationImpl.h

  SOURCES
    sensorAggregation.cpp
    src/sensorAggregationImpl.cpp

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
