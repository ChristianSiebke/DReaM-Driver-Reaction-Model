set(COMPONENT_NAME AgentUpdater)

add_compile_definitions(AGENT_UPDATER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    agentUpdater.h
    src/agentUpdaterImpl.h

  SOURCES
    agentUpdater.cpp
    src/agentUpdaterImpl.cpp

  LIBRARIES
    Qt5::Core
)
