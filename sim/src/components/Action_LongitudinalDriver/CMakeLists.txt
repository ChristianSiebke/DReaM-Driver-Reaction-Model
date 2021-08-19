set(COMPONENT_NAME Action_LongitudinalDriver)

add_compile_definitions(ACTION_LONGITUDINAL_DRIVER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    action_longitudinalDriver.h
    src/longitudinalDriver.h

  SOURCES
    action_longitudinalDriver.cpp
    src/longitudinalDriver.cpp

  LIBRARIES
    Qt5::Core
)
