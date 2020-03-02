set(COMPONENT_TEST_NAME LimiterAccVehComp_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/Components/LimiterAccelerationVehicleComponents)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test
  DEFAULT_MAIN

  SOURCES
    limiterAccVehComp_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/limiterAccelerationVehicleComponentsImplementation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/limiterAccelerationVehicleComponentsImplementation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

