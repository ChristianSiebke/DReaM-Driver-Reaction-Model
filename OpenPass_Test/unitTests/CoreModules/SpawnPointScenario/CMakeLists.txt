set(COMPONENT_TEST_NAME SpawnPointScenarioOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreModules/SpawnPoints/SpawnPointScenario)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnPointScenario_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/SpawnPointScenario.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/agentBlueprintProvider.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicProfileSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicParametersSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicAgentTypeGenerator.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/sampler.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agentType.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/componentType.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelLibrary.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/SpawnPointScenario.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/agentBlueprintProvider.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/agentDataPublisher.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicProfileSampler.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicParametersSampler.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/dynamicAgentTypeGenerator.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework/sampler.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agent.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/agentType.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/channel.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/component.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements/componentType.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelBinding.h
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface/modelLibrary.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/CoreShare
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/framework
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/importer
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelElements
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/modelInterface
    ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave/spawnPointInterface

  LIBRARIES
    Qt5::Core
    CoreShare
)

