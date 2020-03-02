set(COMPONENT_NAME SpawnPoint_Start)

add_compile_definitions(SPAWNPOINT_START_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    spawnpoint_start.h
    spawnpoint_start_global.h
    spawnpoint_start_implementation.h

  SOURCES
    spawnpoint_start.cpp
    spawnpoint_start_implementation.cpp

  LIBRARIES
    Qt5::Core
)
