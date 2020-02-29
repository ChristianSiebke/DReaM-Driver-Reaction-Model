SET (SOURCES dynamics_collision.cpp
			 dynamics_collisionImplementation.cpp)
SET (HEADERS dynamics_collision.h
			 dynamics_collisionImplementation.h
			 dynamics_collisionGlobal.h)

add_definitions(-DDYNAMICS_COLLISION_LIBRARY)

OPENPASS_add_component(Dynamics_Collision "")
