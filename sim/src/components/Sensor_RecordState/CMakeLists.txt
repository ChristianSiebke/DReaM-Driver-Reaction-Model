set(COMPONENT_NAME Sensor_RecordState)

add_compile_definitions(SENSOR_RECORD_STATE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensor_recordState.h
    src/sensor_recordStateImpl.h

  SOURCES
    sensor_recordState.cpp
    src/sensor_recordStateImpl.cpp

  LIBRARIES
    Qt5::Core
)
