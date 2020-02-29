SET (SOURCES objectDetectorBase.cpp
			 sensorGeometric2D.cpp
			 sensorObjectDetectorFactory.cpp)
SET (HEADERS objectDetectorBase.h
			 sensorGeometric2D.h
			 sensorObjectDetectorFactory.h
			 sensorObjectDetectorGlobal.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

add_definitions(-DSENSOR_OBJECT_DETECTOR_LIBRARY)

OPENPASS_add_component(Sensor_OSI "")

target_link_libraries(Sensor_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})