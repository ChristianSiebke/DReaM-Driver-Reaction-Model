SET (SOURCES observationCyclics.cpp
			 observationFileHandler.cpp
			 observation_log.cpp
			 observation_logImplementation.cpp
			 runStatistic.cpp
			 runStatisticCalculation.cpp)
SET (HEADERS observationCyclics.h
			 observationFileHandler.h
			 observationLogConstants.h
			 observationLogGlobal.h
			 observation_log.h
			 observation_logImplementation.h
			 runStatistic.h
			 runStatisticCalculation.h)


add_definitions(-DOBSERVATION_LOG_LIBRARY)

OPENPASS_ADD_MODULE(Observation_Log "")
