set(COMPONENT_NAME Dynamics_RegularTwoTrack)

add_compile_definitions(DYNAMICS_REGULAR_TWOTRACK_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_regularTwoTrack.h
    dynamics_regularTwoTrackImplementation.h
    dynamics_regularTwoTrackGlobal.h
    src/dynamics_twotrack_tire.h
    src/dynamics_twotrack_vehicle.h

  SOURCES
    dynamics_regularTwoTrack.cpp
    dynamics_regularTwoTrackImplementation.cpp
    src/dynamics_twotrack_tire.cpp
    src/dynamics_twotrack_vehicle.cpp

  LIBRARIES
    Qt5::Core

  INCDIRS
    src
)
