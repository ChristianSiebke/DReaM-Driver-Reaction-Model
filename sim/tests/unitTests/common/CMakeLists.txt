set(COMPONENT_TEST_NAME Common_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/common)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINK_OSI

  SOURCES
    commonHelper_Tests.cpp
    ttcCalculation_Tests.cpp
    tokenizeString_Tests.cpp
    vectorToString_Tests.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/commonTools.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Common
)
