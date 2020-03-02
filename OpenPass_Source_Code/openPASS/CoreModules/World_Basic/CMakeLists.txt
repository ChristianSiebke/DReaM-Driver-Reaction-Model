set(COMPONENT_NAME World_Basic)

add_compile_definitions(WORLD_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    agentAdapter.h
    agentNetwork.h
    world_basic.h
    world_basic_implementation.h
    world_basic_global.h

  SOURCES
    agentAdapter.cpp
    agentNetwork.cpp
    world_basic.cpp
    world_basic_implementation.cpp

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
)
