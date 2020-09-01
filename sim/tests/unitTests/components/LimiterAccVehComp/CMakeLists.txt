set(COMPONENT_TEST_NAME LimiterAccVehComp_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/LimiterAccVehComp/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    limiterAccVehComp_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/limiterAccVehCompImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/limiterAccVehCompImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

