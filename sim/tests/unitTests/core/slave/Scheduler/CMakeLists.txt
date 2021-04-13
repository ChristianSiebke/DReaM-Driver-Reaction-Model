set(COMPONENT_TEST_NAME Scheduler_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/framework/scheduler)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    ${COMPONENT_SOURCE_DIR}/agentParser.cpp
    ${COMPONENT_SOURCE_DIR}/runResult.cpp
    ${COMPONENT_SOURCE_DIR}/scheduler.cpp
    ${COMPONENT_SOURCE_DIR}/schedulerTasks.cpp
    ${COMPONENT_SOURCE_DIR}/taskBuilder.cpp
    ${COMPONENT_SOURCE_DIR}/tasks.cpp
    ${OPENPASS_SIMCORE_DIR}/common/eventDetectorDefinitions.cpp
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/eventDetectorBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/eventDetectorLibrary.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agent.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/agentBlueprint.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/channel.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/component.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements/parameters.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelBinding.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings/modelLibrary.cpp
    taskBuilder_Tests.cpp
    schedulerTasks_Tests.cpp
    agentParser_Tests.cpp
    scheduler_Tests.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h
    ${COMPONENT_SOURCE_DIR}/agentParser.h
    ${COMPONENT_SOURCE_DIR}/runResult.h
    ${COMPONENT_SOURCE_DIR}/scheduler.h
    ${COMPONENT_SOURCE_DIR}/schedulerTasks.h
    ${COMPONENT_SOURCE_DIR}/taskBuilder.h
    ${COMPONENT_SOURCE_DIR}/tasks.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/slave
    ${OPENPASS_SIMCORE_DIR}/core/slave/bindings
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelElements
    ${OPENPASS_SIMCORE_DIR}/core/slave/modelInterface

  LIBRARIES
    Qt5::Core
)
