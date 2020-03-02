set(COMPONENT_NAME Algorithm_Longitudinal)

add_compile_definitions(ALGORITHM_LONGITUDINAL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    algorithm_longitudinal.h
    algorithm_longitudinalCalculations.h
    algorithm_longitudinalGlobal.h
    algorithm_longitudinalImplementation.h

  SOURCES
    algorithm_longitudinal.cpp
    algorithm_longitudinalCalculations.cpp
    algorithm_longitudinalImplementation.cpp

  LIBRARIES
    Qt5::Core
)
