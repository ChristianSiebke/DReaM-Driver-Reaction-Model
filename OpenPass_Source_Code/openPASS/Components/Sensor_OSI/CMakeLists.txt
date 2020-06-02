set(COMPONENT_NAME Sensor_OSI)

add_compile_definitions(SENSOR_OBJECT_DETECTOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    objectDetectorBase.h
    sensorGeometric2D.h
    sensorObjectDetectorFactory.h
    sensorObjectDetectorGlobal.h

  SOURCES
    objectDetectorBase.cpp
    sensorGeometric2D.cpp
    sensorObjectDetectorFactory.cpp

  INCDIRS
    ../../CoreModules/World_OSI

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
