SET (SOURCES limiterAccelerationVehicleComponents.cpp
			 limiterAccelerationVehicleComponentsImplementation.cpp)
SET (HEADERS limiterAccelerationVehicleComponents.h
			 limiterAccelerationVehicleComponentsImplementation.h
			 limiterAccelerationVehicleComponentsGlobal.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DLIMITER_ACCELERATION_VEHICLE_COMPONENTS_LIBRARY)

OPENPASS_add_component(LimiterAccelerationVehicleComponents "")

target_link_libraries(LimiterAccelerationVehicleComponents Common ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})