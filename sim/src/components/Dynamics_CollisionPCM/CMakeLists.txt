set(COMPONENT_NAME Dynamics_CollisionPCM)

add_compile_definitions(DYNAMICS_COLLISIONPCM_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_collsion.h
    dynamics_collsion_implementation.h
    dynamics_collsion_global.h

  SOURCES
    dynamics_collsion.cpp
    dynamics_collsion_implementation.cpp

  LIBRARIES
    Qt5::Core
)
