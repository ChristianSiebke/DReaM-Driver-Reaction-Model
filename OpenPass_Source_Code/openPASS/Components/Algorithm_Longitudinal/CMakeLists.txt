SET (SOURCES algorithm_longitudinal.cpp
			 algorithm_longitudinalCalculations.cpp
			 algorithm_longitudinalImplementation.cpp)
SET (HEADERS algorithm_longitudinal.h
			 algorithm_longitudinalCalculations.h
			 algorithm_longitudinalGlobal.h
			 algorithm_longitudinalImplementation.h)


add_definitions(-DALGORITHM_LONGITUDINAL_LIBRARY)

OPENPASS_add_component(Algorithm_Longitudinal "")