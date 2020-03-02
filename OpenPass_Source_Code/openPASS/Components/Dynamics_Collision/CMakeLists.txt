set(COMPONENT_NAME Dynamics_Collision)

add_compile_definitions(DYNAMICS_COLLISION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    dynamics_collision.h
    dynamics_collisionImplementation.h
    dynamics_collisionGlobal.h

  SOURCES
    dynamics_collision.cpp
    dynamics_collisionImplementation.cpp

  LIBRARIES
    Qt5::Core
)
