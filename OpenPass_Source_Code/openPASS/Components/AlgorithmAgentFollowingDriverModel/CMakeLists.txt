set(COMPONENT_NAME AlgorithmAgentFollowingDriverModel)

add_compile_definitions(ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    AlgorithmAgentFollowingDriverModel.h
    AlgorithmAgentFollowingDriverModelGlobal.h
    AlgorithmAgentFollowingDriverModelImplementation.h

  SOURCES
    AlgorithmAgentFollowingDriverModel.cpp
    AlgorithmAgentFollowingDriverModelImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
