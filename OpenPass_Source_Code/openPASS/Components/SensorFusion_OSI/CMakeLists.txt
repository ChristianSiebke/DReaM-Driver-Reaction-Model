SET (SOURCES sensorFusion.cpp
			 sensorFusionImplementation.cpp)
SET (HEADERS sensorFusion.h
			 sensorFusionGlobal.h
			 sensorFusionImplementation.h
			 sensorFusionQuery.h)

add_definitions(-DSENSOR_FUSION_LIBRARY)

include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})


OPENPASS_add_component(SensorFusion_OSI OSI)

target_link_libraries(SensorFusion_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})