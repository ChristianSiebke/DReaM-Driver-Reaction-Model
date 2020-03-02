set(COMPONENT_TEST_NAME Spawner_IntegrationTests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreModules/SpawnPoints)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelLibrary.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/sampler.cpp
    ${COMPONENT_SOURCE_DIR}/Common/WorldAnalyzer.cpp
    ${COMPONENT_SOURCE_DIR}/SpawnPointPreRunCommon/SpawnPointPreRunCommon.cpp
    Spawner_IntegrationTests.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/sampler.h
    ${COMPONENT_SOURCE_DIR}/Common/WorldAnalyzer.h
    ${COMPONENT_SOURCE_DIR}/SpawnPointPreRunCommon/SpawnPointPreRunCommon.h

  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/CoreShare
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/observationInterface
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/spawnPointInterface
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/Common
    ${COMPONENT_SOURCE_DIR}/SpawnPointPreRunCommon

  LIBRARIES
    Qt5::Core
    CoreShare
)

