set(COMPONENT_NAME Algorithm_Lateral)

add_compile_definitions(ALGORITHM_LATERAL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_lateral.h
    src/lateralImpl.h

  SOURCES
    algorithm_lateral.cpp
    src/lateralImpl.cpp

  LIBRARIES
    Qt5::Core
    Common
)

