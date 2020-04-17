set(COMPONENT_NAME Parameters_Vehicle)

add_compile_definitions(PARAMETERS_VEHICLE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    parameters_vehicle.h
    parameters_vehicleGlobal.h
    parameters_vehicleImplementation.h

  SOURCES
    parameters_vehicle.cpp
    parameters_vehicleImplementation.cpp

  LIBRARIES
    Qt5::Core
)
