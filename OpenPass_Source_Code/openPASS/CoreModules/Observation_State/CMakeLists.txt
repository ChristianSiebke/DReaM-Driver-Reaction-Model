SET (SOURCES observation_state.cpp
			 observation_state_implementation.cpp)
SET (HEADERS observation_state.h
			 observation_state_implementation.h
			 observation_state_global.h
			 observationtypes.h)


add_definitions(-DOBSERVATION_STATE_LIBRARY)

OPENPASS_ADD_MODULE(Observation_State "")
