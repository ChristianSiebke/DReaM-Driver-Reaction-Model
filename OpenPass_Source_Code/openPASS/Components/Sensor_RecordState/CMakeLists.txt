set(COMPONENT_NAME Sensor_RecordState)

add_compile_definitions(SENSOR_RECORD_STATE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    sensor_recordState.h
    sensor_recordStateGlobal.h
    sensor_recordStateImplementation.h

  SOURCES
    sensor_recordState.cpp
    sensor_recordStateImplementation.cpp

  LIBRARIES
    Qt5::Core
)
