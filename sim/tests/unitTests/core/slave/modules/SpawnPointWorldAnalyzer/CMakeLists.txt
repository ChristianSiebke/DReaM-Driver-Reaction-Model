set(COMPONENT_TEST_NAME SpawnPointWorldAnalyzer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/modules/Spawners)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnPointWorldAnalyzer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/common/SpawnPointDefinitions.h
    ${COMPONENT_SOURCE_DIR}/common/WorldAnalyzer.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core

  LIBRARIES
    Qt5::Core
    Common
    CoreCommon
)

