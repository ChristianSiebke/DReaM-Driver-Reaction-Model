set(COMPONENT_TEST_NAME BasicDataStore_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/slave/modules/BasicDataStore)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    basicDataStore_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/basicDataStoreImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/basicDataStoreImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

