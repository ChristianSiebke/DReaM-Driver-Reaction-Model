set(COMPONENT_NAME Sensor_Distance)

add_compile_definitions(SENSOR_DISTANCE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensor_distance.h
    sensor_distance_implementation.h
    sensor_distance_global.h

  SOURCES
    sensor_distance.cpp
    sensor_distance_implementation.cpp

  LIBRARIES
    Qt5::Core
)
