set(COMPONENT_NAME Observation_EntityRepository)

add_compile_definitions(OBSERVATION_ENTITYREPOSITORY_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    observation_entityRepositoryGlobal.h
    observation_entityRepository.h
    observation_entityRepositoryImplementation.h

  SOURCES
    observation_entityRepository.cpp
    observation_entityRepositoryImplementation.cpp

  LIBRARIES
    Qt5::Core
)
