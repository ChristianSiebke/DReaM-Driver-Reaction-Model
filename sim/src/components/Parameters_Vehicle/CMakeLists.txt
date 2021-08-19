set(COMPONENT_NAME Parameters_Vehicle)

add_compile_definitions(PARAMETERS_VEHICLE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    parameters_vehicle.h
    src/parameters_vehicleImpl.h

  SOURCES
    parameters_vehicle.cpp
    src/parameters_vehicleImpl.cpp

  LIBRARIES
    Qt5::Core
)
