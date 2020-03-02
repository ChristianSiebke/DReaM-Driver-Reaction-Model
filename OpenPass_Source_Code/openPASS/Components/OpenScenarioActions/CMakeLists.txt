set(COMPONENT_NAME OpenScenarioActions)

add_compile_definitions(OPENSCENARIO_ACTIONS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    openScenarioActions.h
    openScenarioActionsGlobal.h
    openScenarioActionsImplementation.h

  SOURCES
    openScenarioActions.cpp
    openScenarioActionsImplementation.cpp

  LIBRARIES
    Qt5::Core
)
