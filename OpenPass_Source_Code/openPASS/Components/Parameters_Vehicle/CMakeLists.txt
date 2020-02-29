SET (SOURCES parameters_vehicle.cpp
			 parameters_vehicleImplementation.cpp)
SET (HEADERS parameters_vehicle.h
			 parameters_vehicleGlobal.h
			 parameters_vehicleImplementation.h)

add_definitions(-DPARAMETERS_VEHICLE_LIBRARY)

OPENPASS_add_component(Parameters_Vehicle "")
