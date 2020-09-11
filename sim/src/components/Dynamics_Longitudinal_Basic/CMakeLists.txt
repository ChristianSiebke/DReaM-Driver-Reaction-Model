set(COMPONENT_NAME Dynamics_Longitudinal_Basic)

add_compile_definitions(DYNAMICS_LONGITUDINAL_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_longitudinal_basic.h
    dynamics_longitudinal_basic_implementation.h
    dynamics_longitudinal_basic_global.h

  SOURCES
    dynamics_longitudinal_basic.cpp
    dynamics_longitudinal_basic_implementation.cpp

  INCDIRS
    ../../../include
    ../../common

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
