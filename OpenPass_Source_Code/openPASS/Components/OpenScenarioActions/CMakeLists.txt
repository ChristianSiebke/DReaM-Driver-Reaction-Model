set(COMPONENT_NAME OpenScenarioActions)

add_compile_definitions(OPENSCENARIO_ACTIONS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    openScenarioActions.h
    oscActionsCalculation.h
    openScenarioActionsGlobal.h
    openScenarioActionsImplementation.h
    oscActionsCalculation.h
    actionTransformRepository.h
    transformerBase.h
    transformTrajectory.h
    transformLaneChange.h
    transformSpeedAction.h

  SOURCES
    openScenarioActions.cpp
    oscActionsCalculation.cpp
    openScenarioActionsImplementation.cpp
    oscActionsCalculation.cpp
    transformLaneChange.cpp
    transformSpeedAction.cpp

  LIBRARIES
    Qt5::Core
)
