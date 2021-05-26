set(COMPONENT_NAME Sensor_Collision)

add_compile_definitions(SENSOR_COLLISION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensor_collsion.h
    sensor_collsion_implementation.h
    sensor_collsion_global.h

  SOURCES
    sensor_collsion.cpp
    sensor_collsion_implementation.cpp

  LIBRARIES
    Qt5::Core
)
