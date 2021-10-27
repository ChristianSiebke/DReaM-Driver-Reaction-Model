set(COMPONENT_NAME Dynamics_Collision)

add_compile_definitions(DYNAMICS_COLLISION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_collision.h
    src/collisionImpl.h

  SOURCES
    dynamics_collision.cpp
    src/collisionImpl.cpp

  LIBRARIES
    Qt5::Core
)
