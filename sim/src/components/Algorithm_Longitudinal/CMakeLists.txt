set(COMPONENT_NAME Algorithm_Longitudinal)

add_compile_definitions(ALGORITHM_LONGITUDINAL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_longitudinal.h
    src/longCalcs.h
    src/algo_longImpl.h

  SOURCES
    algorithm_longitudinal.cpp
    src/longCalcs.cpp
    src/algo_longImpl.cpp

  LIBRARIES
    Qt5::Core
)
