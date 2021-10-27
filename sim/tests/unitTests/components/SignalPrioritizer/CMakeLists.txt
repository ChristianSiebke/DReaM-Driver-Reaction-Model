set(COMPONENT_TEST_NAME SignalPrioritizer_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/SignalPrioritizer/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    signalPrioritizer_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/signalPrioritizerImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/signalPrioritizerImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

