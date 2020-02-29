SET (SOURCES sensor_recordState.cpp
			 sensor_recordStateImplementation.cpp)
SET (HEADERS sensor_recordState.h
			 sensor_recordStateGlobal.h
			 sensor_recordStateImplementation.h)

add_definitions(-DSENSOR_RECORD_STATE_LIBRARY)

OPENPASS_add_component(Sensor_RecordState "")
