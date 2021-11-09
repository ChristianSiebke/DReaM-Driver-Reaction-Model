set(COMPONENT_NAME Algorithm_AEB)

add_compile_definitions(ALGORITHM_AEB_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    algorithm_autonomousEmergencyBraking.h
    src/autonomousEmergencyBraking.h

  SOURCES
    algorithm_autonomousEmergencyBraking.cpp
    src/autonomousEmergencyBraking.cpp

  INCDIRS

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common

  LINKOSI
)
