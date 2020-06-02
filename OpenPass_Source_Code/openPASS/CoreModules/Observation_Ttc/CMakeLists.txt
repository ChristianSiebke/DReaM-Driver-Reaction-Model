set(COMPONENT_NAME Observation_Ttc)

add_compile_definitions(OBSERVATION_TTC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    observation_ttc.h
    observation_ttc_implementation.h
    observation_ttc_global.h

  SOURCES
    observation_ttc.cpp
    observation_ttc_implementation.cpp

  INCDIRS
    ../../Common
    ../../Interfaces

  LIBRARIES
    Qt5::Core
)
