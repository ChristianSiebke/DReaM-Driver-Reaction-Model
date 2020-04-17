set(COMPONENT_NAME Algorithm_AEB)

add_compile_definitions(ALGORITHM_AEB_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_autonomousEmergencyBraking.h
    algorithm_autonomousEmergencyBrakingGlobal.h
    algorithm_autonomousEmergencyBrakingImplementation.h
    boundingBoxCalculation.h

  SOURCES
    algorithm_autonomousEmergencyBraking.cpp
    algorithm_autonomousEmergencyBrakingImplementation.cpp
    boundingBoxCalculation.cpp

  INCDIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../CoreModules/World_OSI

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common

  LINKOSI
)
