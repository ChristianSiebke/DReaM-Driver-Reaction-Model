set(COMPONENT_NAME LimiterAccelerationVehicleComponents)

add_compile_definitions(LIMITER_ACCELERATION_VEHICLE_COMPONENTS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    limiterAccVehComp.h
    src/limiterAccVehCompImpl.h

  SOURCES
    limiterAccVehComp.cpp
    src/limiterAccVehCompImpl.cpp

  LIBRARIES
    Qt5::Core
    Common
)
