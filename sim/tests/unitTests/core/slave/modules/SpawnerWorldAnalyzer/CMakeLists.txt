set(COMPONENT_TEST_NAME SpawnerWorldAnalyzer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/modules/Spawners)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnerWorldAnalyzer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.cpp
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/common/SpawnerDefinitions.h
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.h
    ${OPENPASS_SIMCORE_DIR}/common/RoutePlanning/RouteCalculation.h
    ${OPENPASS_SIMCORE_DIR}/core/slave/framework/sampler.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/core/slave
    ${OPENPASS_SIMCORE_DIR}/common

  LIBRARIES
    Qt5::Core
    Common
    CoreCommon
)

