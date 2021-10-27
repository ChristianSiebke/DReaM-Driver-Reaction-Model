set(COMPONENT_NAME Dynamics_RegularDriving)

add_compile_definitions(DYNAMICS_REGULAR_DRIVING_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_regularDriving.h
    src/regularDriving.h

  SOURCES
    dynamics_regularDriving.cpp
    src/regularDriving.cpp

  LIBRARIES
    Qt5::Core
)
