SET (SOURCES algorithm_cruisecontrolbydistance.cpp
			 algorithm_cruisecontrolbydistance_implementation.cpp)
SET (HEADERS algorithm_cruisecontrolbydistance.h
			 algorithm_cruisecontrolbydistance_global.h
			 algorithm_cruisecontrolbydistance_implementation.h)


add_definitions(-DALGORITHM_CRUISECONTROLBYDISTANCE_LIBRARY)

OPENPASS_add_component(Algorithm_CruiseControlByDistance "")