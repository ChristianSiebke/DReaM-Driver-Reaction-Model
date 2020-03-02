set(COMPONENT_NAME SpawnPointRuntimeCommon)

add_compile_definitions(SPAWNPOINT_COMMON_RUNTIME_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h
    ../Common/SpawnPointDefinitions.h
    ../Common/WorldAnalyzer.h
    SpawnPointRuntimeCommon.h
    SpawnPointRuntimeCommonExport.h
    SpawnPointRuntimeCommonGlobal.h
    SpawnPointRuntimeCommonParameterExtractor.h

  SOURCES
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    ../Common/WorldAnalyzer.cpp
    SpawnPointRuntimeCommon.cpp
    SpawnPointRuntimeCommonExport.cpp

  INCDIRS
    ..
    ../../../CoreFramework/CoreShare
    ../../../CoreFramework/OpenPassSlave/importer
    ../../../CoreFramework/OpenPassSlave/modelElements

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreShare
)
