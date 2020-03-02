set(COMPONENT_NAME Algorithm_CruiseControlByDistance)

add_compile_definitions(ALGORITHM_CRUISECONTROLBYDISTANCE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    algorithm_cruisecontrolbydistance.h
    algorithm_cruisecontrolbydistance_global.h
    algorithm_cruisecontrolbydistance_implementation.h

  SOURCES
    algorithm_cruisecontrolbydistance.cpp
    algorithm_cruisecontrolbydistance_implementation.cpp

  LIBRARIES
    Qt5::Core
)
