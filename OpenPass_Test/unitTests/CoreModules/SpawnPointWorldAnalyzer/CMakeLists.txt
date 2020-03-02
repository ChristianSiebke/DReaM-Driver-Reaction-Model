set(COMPONENT_TEST_NAME SpawnPointWorldAnalyzer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreModules/SpawnPoints)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    spawnPointWorldAnalyzer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/Common/WorldAnalyzer.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/Common/SpawnPointDefinitions.h
    ${COMPONENT_SOURCE_DIR}/Common/WorldAnalyzer.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)

