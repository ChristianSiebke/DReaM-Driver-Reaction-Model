set(COMPONENT_TEST_NAME SpawnPointScenario_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/modules/Spawners/Scenario)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnPointScenario_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/SpawnPointScenario.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentBlueprintProvider.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicProfileSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicParametersSampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicAgentTypeGenerator.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentType.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/componentType.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelLibrary.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/SpawnPointScenario.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentBlueprintProvider.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentDataPublisher.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicProfileSampler.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicParametersSampler.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/dynamicAgentTypeGenerator.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agent.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentBlueprint.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentType.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/channel.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/component.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/componentType.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelBinding.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelLibrary.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/..
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/slave
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework
    ${OPENPASS_SIMCORE_DIR}/core/slave/importer
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements

  LIBRARIES
    Qt5::Core
    CoreCommon
)
