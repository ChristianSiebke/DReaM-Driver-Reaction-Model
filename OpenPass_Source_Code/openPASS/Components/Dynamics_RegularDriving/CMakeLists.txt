SET (SOURCES dynamics_regularDriving.cpp
			 dynamics_regularDrivingImplementation.cpp)
SET (HEADERS dynamics_regularDriving.h
			 dynamics_regularDrivingImplementation.h
			 dynamics_regularDrivingGlobal.h)

add_definitions(-DDYNAMICS_REGULAR_DRIVING_LIBRARY)

OPENPASS_add_component(Dynamics_RegularDriving "")
