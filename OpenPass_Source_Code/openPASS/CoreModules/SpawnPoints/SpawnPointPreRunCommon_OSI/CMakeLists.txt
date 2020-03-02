set(COMPONENT_NAME SpawnPointPreRunCommon)

add_compile_definitions(SPAWNPOINT_COMMON_PRERUN_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h
    ../Common/SpawnPointDefinitions.h
    ../Common/WorldAnalyzer.h
    SpawnPointPreRunCommon.h
    SpawnPointPreRunCommonExport.h
    SpawnPointPreRunCommonGlobal.h
    SpawnPointPreRunCommonParameterExtractor.h

  SOURCES
    ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    ../Common/WorldAnalyzer.cpp
    SpawnPointPreRunCommon.cpp
    SpawnPointPreRunCommonExport.cpp
    SpawnPointPreRunCommonParameterExtractor.cpp

  INCDIRS
    ..
    ../../../CoreFramework/OpenPassSlave/modelElements

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    CoreShare
)
