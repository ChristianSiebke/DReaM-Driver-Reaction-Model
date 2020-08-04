set(COMPONENT_NAME Observation_LogAgent)

add_compile_definitions(OBSERVATION_LOGAGENT_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    observationCyclics.h
    observationFileHandler.h
    observationLogConstants.h
    observation_LogGlobal.h
    observation_log.h
    observation_logImplementation.h
    runStatistic.h
    runStatisticCalculation.h

  SOURCES
    observationCyclics.cpp
    observationFileHandler.cpp
    observation_log.cpp
    observation_logImplementation.cpp
    runStatistic.cpp
    runStatisticCalculation.cpp

  LIBRARIES
    Qt5::Core
)
