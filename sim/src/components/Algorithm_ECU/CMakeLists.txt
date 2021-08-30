set(COMPONENT_NAME Algorithm_ECU)

add_compile_definitions(ALGORITHM_ECU_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_ECU.h
    algorithm_ECU_implementation.h
    algorithm_ECU_global.h
    ecu.h

  SOURCES
    algorithm_ECU.cpp
    algorithm_ECU_implementation.cpp
    ecu.cpp

  LIBRARIES
    Qt5::Core
)
