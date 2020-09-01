set(COMPONENT_NAME AlgorithmAgentFollowingDriverModel)

add_compile_definitions(ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    AlgorithmAFDM.h
    src/followingDriverModel.h

  SOURCES
    AlgorithmAFDM.cpp
    src/followingDriverModel.cpp

  LIBRARIES
    Qt5::Core
    Common
)
