set(COMPONENT_NAME Algorithm_Lateral)

add_compile_definitions(ALGORITHM_LATERAL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_lateral.h
    algorithm_lateralGlobal.h
    algorithm_lateralImplementation.h

  SOURCES
    algorithm_lateral.cpp
    algorithm_lateralImplementation.cpp

  LIBRARIES
    Qt5::Core
    Common
)

