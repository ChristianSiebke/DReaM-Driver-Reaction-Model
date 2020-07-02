set(COMPONENT_NAME OpenScenarioActions)

add_compile_definitions(OPENSCENARIO_ACTIONS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    openScenarioActions.h
    openScenarioActionsGlobal.h
    openScenarioActionsImplementation.h
    oscActionsCalculation.h

  SOURCES
    openScenarioActions.cpp
    openScenarioActionsImplementation.cpp
    oscActionsCalculation.cpp
    
  LIBRARIES
    Qt5::Core
)
