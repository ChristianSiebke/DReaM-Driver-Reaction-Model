set(COMPONENT_NAME Dynamics_CollisionPCM)

add_compile_definitions(DYNAMICS_COLLISIONPCM_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_collision.h
    dynamics_collision_implementation.h
    dynamics_collision_global.h

  SOURCES
    dynamics_collision.cpp
    dynamics_collision_implementation.cpp

  LIBRARIES
    Qt5::Core
)
