set(COMPONENT_NAME Dynamics_Chassis)

add_compile_definitions(DYNAMICS_CHASSIS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_Chassis.h
    dynamics_Chassis_implementation.h
    dynamics_Chassis_global.h
	ForceWheelZ.h
	WheelOscillation.h

  SOURCES
    dynamics_Chassis.cpp
    dynamics_Chassis_implementation.cpp
	ForceWheelZ.cpp
	WheelOscillation.cpp

  LIBRARIES
    Qt5::Core
)
