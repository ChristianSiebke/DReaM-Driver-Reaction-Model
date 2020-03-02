set(COMPONENT_NAME Dynamics_RegularDriving)

add_compile_definitions(DYNAMICS_REGULAR_DRIVING_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    dynamics_regularDriving.h
    dynamics_regularDrivingImplementation.h
    dynamics_regularDrivingGlobal.h

  SOURCES
    dynamics_regularDriving.cpp
    dynamics_regularDrivingImplementation.cpp

  LIBRARIES
    Qt5::Core
)
