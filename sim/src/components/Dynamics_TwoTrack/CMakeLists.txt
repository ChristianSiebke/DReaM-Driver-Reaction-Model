set(COMPONENT_NAME Dynamics_TwoTrack)

add_compile_definitions(DYNAMICS_TWOTRACK_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_twotrack.h
    dynamics_twotrack_implementation.h
    dynamics_twotrack_global.h
    src/dynamics_twotrack_vehicle.h
    src/dynamics_twotrack_tire.h
    ../../common/pcm/controlSignal.h

  SOURCES
    dynamics_twotrack.cpp
    dynamics_twotrack_implementation.cpp
    src/dynamics_twotrack_vehicle.cpp
    src/dynamics_twotrack_tire.cpp
    ../../common/pcm/controlSignal.cpp

  INCDIRS
    src
    ../../common/pcm

  LIBRARIES
    Qt5::Core
)
