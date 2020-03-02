set(COMPONENT_NAME CollisionDetection)

add_compile_definitions(COLLISIONDETECTION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    collisionDetection.h
    collisionDetection_implementation.h
    collisionDetection_global.h

  SOURCES
    collisionDetection.cpp
    collisionDetection_implementation.cpp

  LIBRARIES
    Qt5::Core
)
