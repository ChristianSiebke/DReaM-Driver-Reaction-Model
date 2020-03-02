set(COMPONENT_NAME LimiterAccelerationVehicleComponents)

add_compile_definitions(LIMITER_ACCELERATION_VEHICLE_COMPONENTS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    limiterAccelerationVehicleComponents.h
    limiterAccelerationVehicleComponentsImplementation.h
    limiterAccelerationVehicleComponentsGlobal.h

  SOURCES
    limiterAccelerationVehicleComponents.cpp
    limiterAccelerationVehicleComponentsImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common
)
