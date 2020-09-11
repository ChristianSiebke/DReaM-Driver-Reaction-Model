set(COMPONENT_NAME Algorithm_AEB)

add_compile_definitions(ALGORITHM_AEB_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_autonomousEmergencyBraking.h
    src/autonomousEmergencyBraking.h
    src/boundingBoxCalculation.h

  SOURCES
    algorithm_autonomousEmergencyBraking.cpp
    src/autonomousEmergencyBraking.cpp
    src/boundingBoxCalculation.cpp

  INCDIRS
    ${CMAKE_CURRENT_LIST_DIR}/core/slave/modules/World_OSI

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common

  LINKOSI
)
