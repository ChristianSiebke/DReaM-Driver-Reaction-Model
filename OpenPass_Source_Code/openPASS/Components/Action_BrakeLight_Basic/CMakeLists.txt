set(COMPONENT_NAME Action_BrakeLight_Basic)

add_compile_definitions(ACTION_BRAKELIGHT_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    action_brakelight_basic.h
    action_brakelight_basic_implementation.h
    action_brakelight_basic_global.h

  SOURCES
    action_brakelight_basic.cpp
    action_brakelight_basic_implementation.cpp

  LIBRARIES
    Qt5::Core
)
