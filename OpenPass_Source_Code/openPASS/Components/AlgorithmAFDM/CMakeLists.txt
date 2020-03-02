set(COMPONENT_NAME AlgorithmAgentFollowingDriverModel)

add_compile_definitions(ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    AlgorithmAFDM.h
    AlgorithmAFDMGlobal.h
    AlgorithmAFDMImplementation.h

  SOURCES
    AlgorithmAFDM.cpp
    AlgorithmAFDMImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common
)
