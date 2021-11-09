set(COMPONENT_NAME Observation_State)

add_compile_definitions(OBSERVATION_STATE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    observation_state.h
    observation_state_implementation.h
    observation_state_global.h
    observationtypes.h

  SOURCES
    observation_state.cpp
    observation_state_implementation.cpp

  LIBRARIES
    Qt5::Core
)
