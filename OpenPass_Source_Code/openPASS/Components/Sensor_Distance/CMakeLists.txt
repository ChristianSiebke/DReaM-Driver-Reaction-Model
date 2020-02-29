SET (SOURCES sensor_distance.cpp
			 sensor_distance_implementation.cpp)
SET (HEADERS sensor_distance.h
			 sensor_distance_implementation.h
			 sensor_distance_global.h)

add_definitions(-DSENSOR_DISTANCE_LIBRARY)

OPENPASS_add_component(Sensor_Distance "")
