set(COMPONENT_NAME SpawnPointScenario)

add_compile_definitions(SPAWNPOINT_SCENARIO_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h
    SpawnPointScenario.h
    SpawnPointScenarioExport.h
    SpawnPointScenarioGlobal.h

  SOURCES
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    SpawnPointScenario.cpp
    SpawnPointScenarioExport.cpp

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreShare
)
