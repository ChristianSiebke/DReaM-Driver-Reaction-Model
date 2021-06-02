set(COMPONENT_NAME Dynamics_TwoTrack)

add_compile_definitions(DYNAMICS_TWOTRACK_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_twoTrack.h
    dynamics_twoTrack_implementation.h
    dynamics_twoTrack_global.h
    src/dynamics_twoTrack_vehicle.h
    src/dynamics_twoTrack_tire.h
    ../../common/pcm/controlSignal.h

  SOURCES
    dynamics_twoTrack.cpp
    dynamics_twoTrack_implementation.cpp
    src/dynamics_twoTrack_vehicle.cpp
    src/dynamics_twoTrack_tire.cpp
    ../../common/pcm/controlSignal.cpp

  INCDIRS
    src
    ../../common/pcm

  LIBRARIES
    Qt5::Core
)
