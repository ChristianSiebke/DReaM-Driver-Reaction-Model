set(COMPONENT_NAME SensorFusion_OSI)

add_compile_definitions(SENSOR_FUSION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensorFusion.h
    sensorFusionGlobal.h
    sensorFusionImplementation.h
    sensorFusionQuery.h

  SOURCES
    sensorFusion.cpp
    sensorFusionImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
