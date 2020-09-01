set(COMPONENT_NAME Sensor_OSI)

add_compile_definitions(SENSOR_OBJECT_DETECTOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensorOSI.h
    src/objectDetectorBase.h
    src/sensorGeometric2D.h

  SOURCES
    sensorOSI.cpp
    src/objectDetectorBase.cpp
    src/sensorGeometric2D.cpp

  INCDIRS
    ../../core/slave/modules/World_OSI

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
