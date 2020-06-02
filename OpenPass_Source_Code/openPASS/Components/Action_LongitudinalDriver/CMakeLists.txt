set(COMPONENT_NAME Action_LongitudinalDriver)

add_compile_definitions(ACTION_LONGITUDINAL_DRIVER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    action_longitudinalDriver.h
    action_longitudinalDriverGlobal.h
    action_longitudinalDriverImplementation.h

  SOURCES
    action_longitudinalDriver.cpp
    action_longitudinalDriverImplementation.cpp

  LIBRARIES
    Qt5::Core
)
