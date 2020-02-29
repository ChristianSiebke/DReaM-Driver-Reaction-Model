SET (SOURCES collisionDetection.cpp
			 collisionDetection_implementation.cpp)
SET (HEADERS collisionDetection.h
			 collisionDetection_implementation.h
			 collisionDetection_global.h)


add_definitions(-DCOLLISIONDETECTION_LIBRARY)

OPENPASS_ADD_MODULE(CollisionDetection "")