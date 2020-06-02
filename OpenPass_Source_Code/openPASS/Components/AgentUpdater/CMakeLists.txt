set(COMPONENT_NAME AgentUpdater)

add_compile_definitions(AGENT_UPDATER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    agentUpdater.h
    agentUpdaterGlobal.h
    agentUpdaterImplementation.h

  SOURCES
    agentUpdater.cpp
    agentUpdaterImplementation.cpp

  LIBRARIES
    Qt5::Core
)
