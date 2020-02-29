SET (SOURCES observation_ttc.cpp
			 observation_ttc_implementation.cpp)
SET (HEADERS observation_ttc.h
			 observation_ttc_implementation.h
			 observation_ttc_global.h)


add_definitions(-DOBSERVATION_TTC_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(Observation_Ttc "")