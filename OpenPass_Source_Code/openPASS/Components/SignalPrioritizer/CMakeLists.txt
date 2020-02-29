SET (SOURCES signalPrioritizer.cpp
			 signalPrioritizerImplementation.cpp)
SET (HEADERS signalPrioritizer.h
			 signalPrioritizerGlobal.h
			 signalPrioritizerImplementation.h)

add_definitions(-DSIGNAL_PRIORITIZER_LIBRARY)

OPENPASS_add_component(SignalPrioritizer "")
