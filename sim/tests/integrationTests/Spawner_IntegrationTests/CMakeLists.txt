set(COMPONENT_TEST_NAME Spawner_IntegrationTests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/modules/Spawners)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/parameters.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelLibrary.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.cpp
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.cpp
    ${COMPONENT_SOURCE_DIR}/PreRunCommon/SpawnPointPreRunCommon.cpp
    Spawner_IntegrationTests.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.h
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.h
    ${COMPONENT_SOURCE_DIR}/PreRunCommon/SpawnPointPreRunCommon.h

  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/slave
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements
    ${OPENPASS_SIMCORE_DIR}/core/slave/observationInterface
    ${OPENPASS_SIMCORE_DIR}/core/slave/spawnPointInterface
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/common
    ${COMPONENT_SOURCE_DIR}/PreRunCommon

  LIBRARIES
    Qt5::Core
    CoreCommon
)

